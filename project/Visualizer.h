#pragma once

#include <vector>
#include "ImageRGB.h"

class Visualizer
{
public:
	static void Plot(const std::vector<double>& iY, ImageRGB& ioCanvas);
	static void Plot(const std::vector<double>& iX, const std::vector<double>& iY, ImageRGB	& ioCanvas);
};

