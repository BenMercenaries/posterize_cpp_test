
#include "frame_buffer.h"

#include <vector>
#include <limits>
#include <random>
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>

std::vector<float3> compute_quantization (const frame_buffer &input, int ncolors);


void quantize_image (frame_buffer &input, frame_buffer &output, const std::vector<float3> &colors)
{
	for (int y = 0; y < output.height; ++y)
	{
		for (int x = 0; x < output.width; ++x)
		{
			float3	p = input.get (x, y);

			int		best = -1;
			float	d = std::numeric_limits<float>::max ();
			for (int i = 0; i < colors.size (); ++i)
			{
				float	thisd = distance (p, colors[i]);
				if (thisd < d)
				{
					d = thisd;
					best = i;
				}
			}

			if (best >= 0)
				output.set (x, y, colors[best]);
			else
				output.set (x, y, float3 {0.0f,0.0f,0.0f});
		}
	}
}


std::vector<float3> compute_trivial_quantization (const frame_buffer &input, int ncolors)
{
	// less than 8 colors, just a simple black and white quantization
	if (ncolors < 8)
		return { {0,0,0}, {1,1,1} };

	// compute the colors bounding box
	float3	bmin {+1.0f,+1.0f,+1.0f};
	float3	bmax {+0.0f,+0.0f,+0.0f};

	for (int y = 0; y < input.height; ++y)
	{
		for (int x = 0; x < input.width; ++x)
		{
			float3	p = input.get (x, y);

			bmin.x = std::min (bmin.x, p.x);
			bmin.y = std::min (bmin.y, p.y);
			bmin.z = std::min (bmin.z, p.z);
			bmax.x = std::max (bmax.x, p.x);
			bmax.y = std::max (bmax.y, p.y);
			bmax.z = std::max (bmax.z, p.z);
		}
	}

	std::vector<float3>	colors;

	// get the highest n such that n^3 <= ncolors
	int	quant = 1;
	while (quant*quant*quant <= ncolors)
		++quant;
	--quant;

	// and quantize the color cube
	for (int r = 0; r < quant; ++r)
		for (int g = 0; g < quant; ++g)
			for (int b = 0; b < quant; ++b)
				colors.push_back (
					float3 {
						bmin.x+(bmax.x-bmin.x)*r/(quant-1),
						bmin.y+(bmax.y-bmin.y)*g/(quant-1),
						bmin.z+(bmax.z-bmin.z)*b/(quant-1)
					});

	return colors;
}



int main (int argc, char **argv)
{
	std::string	input = "in.ppm";
	std::string	output = "out.ppm";

	int			ncolors = 16;
	bool		trivial = false;

	for (int i = 1; i < argc; ++i)
	{
		std::string	opt = argv[i];
		if (opt == "-n" && i+1 < argc)
		{
			ncolors = atoi (argv[i+1]);
			++i;
		}
		if (opt == "-t")
		{
			trivial = true;
		}
	}

	// Read the input bitmap
	frame_buffer	infb;
	if (!infb.read_ppm (input.c_str ()))
	{
		std::cerr << "can't read " << input << std::endl;
		return -1;
	}

	// Compuate quantized colors
	std::cout << "Computing color quantization ..." << std::endl;
	std::vector<float3>	colors;
	if (trivial)
		colors = compute_trivial_quantization (infb, ncolors);
	else
		colors = compute_quantization (infb, ncolors);

	// And posterize using quantized colors
	std::cout << "Quantizing image with " << colors.size () << " colors ..." << std::endl;
	frame_buffer	outfb (infb.width, infb.height);
	quantize_image (infb, outfb, colors);

	// Eval the RMSE
	float	rmse = 0.0f;
	for (int y = 0; y < infb.height; ++y)
		for (int x = 0; x < infb.width; ++x)
		{
			float	d = distance (infb.get (x,y), outfb.get (x,y));
			rmse += d*d;
		}
	rmse = std::sqrt (rmse / (infb.width*infb.height));
	std::cout << "Posterization RMSE " << rmse << std::endl;

	// And write the image
	outfb.write_ppm (output.c_str ());

	return 0;	
}
