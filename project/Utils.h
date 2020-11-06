#pragma once

#include "Channel.h"
#include <string>

Channel Load(const std::string& iFilename);
void VisualizeGradient(const Channel& iChannel, const std::string& iFilename);
void ShowGradient(const Channel& iChannel);

void Prewitt(Channel& iChannel, Channel& oPrewittX, Channel& oPrewittY);
void Sobel(Channel& iChannel, Channel& oSobelX, Channel& oSobelY);

Channel Median(Channel& iChannel, size_t iSize);

class Kernels
{
public:
	// docs.gimp.org/2.8/ru/plug-in-convmatrix.html
	static Mask Contrast;

	// desktop.arcgis.com/ru/arcmap/10.3/manage-data/raster-and-images/convolution-function.htm
	static Mask VerticalLines;
	static Mask HorizontalLines;

	static Mask Gauss5x5;
};
