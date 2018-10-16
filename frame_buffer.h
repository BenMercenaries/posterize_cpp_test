
#pragma once

#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string>
#include "float3.h"
#include <assert.h>

class frame_buffer
{
public:
	int	width;
	int height;
	std::vector<float3>	pixels;

	frame_buffer () :
		width (0), height (0)
	{}

	frame_buffer (int w, int h) :
		width (w), height (h), pixels (w*h, float3 {0.0f,0.0f,0.0f})
	{}

	frame_buffer (const frame_buffer &o) = default;
	frame_buffer (frame_buffer &&o) = default;

	frame_buffer &operator = (const frame_buffer &o) = default;
	frame_buffer &operator = (frame_buffer &&o) = default;

	void set (int x, int y, const float3 &v)
	{
		assert (x >= 0 && x < width && y >= 0 && y < height);
		pixels[x+y*width] = v;
	}

	float3 get (int x, int y) const
	{
		assert (x >= 0 && x < width && y >= 0 && y < height);
		return pixels[x+y*width];
	}

	bool read_ppm (const char *file);

	bool write_tga (const char *file) const;
	bool write_ppm (const char *file) const;
};

//#define USE_GAMMA

static inline uint8_t to_image_space (float x)
{
	// clamp to 0-1
	x = std::max (0.0f, std::min (1.0f, x));
#ifdef USE_GAMMA
	// inverse gamma
	x = std::pow (x, 1.0f/2.2f);
#endif
	// quantize
	return uint8_t (std::floor (x * 255.0f));
}

static inline float from_image_space (uint8_t x)
{
#ifdef USE_GAMMA
	return std::pow (x/255.0f, 2.2f);
#else
	return x/255.0f;
#endif
}

inline bool frame_buffer::write_tga (const char *file) const
{
	FILE	*f = fopen (file, "wb");
	if (f == NULL)
		return false;

	uint8_t targaheader[18];

	// Set unused fields of header to 0
	memset (targaheader, 0, sizeof(targaheader));

	targaheader[2] = 3;	// image type = uncompressed RGB
	targaheader[12] = (uint8_t) (width & 0xFF);
	targaheader[13] = (uint8_t) (width >> 8);
	targaheader[14] = (uint8_t) (height & 0xFF);
	targaheader[15] = (uint8_t) (height >> 8);
	targaheader[16] = 24;		// 3x8-bit per pixel
	targaheader[17] = 0x20;		// Top-down, non-interlaced

	// write header
	if (fwrite (targaheader, 18, 1, f) != 1)
	{
		fclose (f);
		return false;
	}

	const float3	*pixel = pixels.data ();

	// write scanlines
	std::vector<uint8_t> buffer (width*3);
	for (int i = 0; i < height; i++)
	{
		uint8_t *dest = buffer.data ();
		for (int j = 0; j < width; j++) 
		{
			dest[0] = to_image_space (pixel->z);
			dest[1] = to_image_space (pixel->y);
			dest[2] = to_image_space (pixel->x);

			++pixel;
			dest += 3;
		}
		if (fwrite (buffer.data (), width*3, 1, f) != 1)
		{
			fclose (f);
			return false;
		}
	}

	fclose (f);
	return true;
}

inline bool frame_buffer::write_ppm (const char *file) const
{
	FILE	*f = fopen (file, "wb");
	if (f == NULL)
		return false;

	fprintf (f, "P6\n%d %d\n255\n", width, height);

	const float3	*pixel = pixels.data ();

	// write scanlines
	std::vector<uint8_t> buffer (width*3);
	for (int i = 0; i < height; i++)
	{
		uint8_t *dest = buffer.data ();
		for (int j = 0; j < width; j++) 
		{
			dest[0] = to_image_space (pixel->x);
			dest[1] = to_image_space (pixel->y);
			dest[2] = to_image_space (pixel->z);

			++pixel;
			dest += 3;
		}
		if (fwrite (buffer.data (), width*3, 1, f) != 1)
		{
			fclose (f);
			return false;
		}
	}

	fclose (f);
	return true;
}

inline bool frame_buffer::read_ppm (const char *file)
{
	FILE	*f = fopen (file, "rb");
	if (f == NULL)
		return false;

	std::string	header;
	int	c;

	// read header
	while ((c = fgetc (f)) != EOF && c != '\n')
		header += c;

	if (header != "P6")
	{
		fclose (f);
		return false;
	}

	do
	{
		header.clear ();
		while ((c = fgetc (f)) != EOF && c != '\n')
			header += c;
	}
	while (header[0] == '#');

	if (sscanf (header.c_str (), "%d %d", &width, &height) != 2)
	{
		fclose (f);
		return false;
	}

	header.clear ();
	while ((c = fgetc (f)) != EOF && c != '\n')
		header += c;

	if (header != "255")
	{
		fclose (f);
		return false;
	}

	pixels.resize (width*height);

	float3	*pixel = pixels.data ();

	// write scanlines
	std::vector<uint8_t> buffer (width*3);
	for (int i = 0; i < height; i++)
	{
		if (fread (buffer.data (), width*3, 1, f) != 1)
		{
			fclose (f);
			return false;
		}

		uint8_t *src = buffer.data ();
		for (int j = 0; j < width; j++) 
		{
			pixel->x = from_image_space (src[0]);
			pixel->y = from_image_space (src[1]);
			pixel->z = from_image_space (src[2]);

			++pixel;
			src += 3;
		}
	}

	fclose (f);
	return true;
}
