
Introduction
============

Hi there!

This is a image color quantizer, an image processor that reduces the number of used colors in an image, while trying to preserve the visual accuracy of the image.

We've provided the basic necessary structs and functions to implement 


Test objective
==============

The objective of this sample is to implement a color quantizer, that is an algorithm that reduces the number of used colors from a source image to a given number. The resulting color set can be smaller than requested, but must not be larger.

You must implement your algorithm in the compute_quantization function in the quantize.cpp file. A trivial implementation is provided as compute_trivial_quantization in main.cpp

Your implementation should be fairly simple, easy to read, and should behave predictably relative to complexity.

You should not over-optimize, nor over-complexify.


Compiling
=========

This sample is standalone (as it does not depend on external libraries) and should compile easily with any c++11 compiler.

## Linux ######
A makefile is provided. A decently modern g++ will do (gcc 4.8 and above.)
'make' will compile in release mode (-O3 -DNDEBUG.)
Compile in debug with 'make VERSION=debug'
The release (resp. debug) executable is release/posterize (resp. debug/posterize)

## Windows ######
A VS2015 solution is provided.
Build the solution using the build menu.

As is, the compute_quantization does nothing and the resulting image is black. You may use the -t option to render the image using the trivial implementation.


Running
=======

The executable takes a few options:

  * -t to quantize using the trivial quantizer
  * -n _ncolors_ to specify the maximum number of the quantization (default is 16)
