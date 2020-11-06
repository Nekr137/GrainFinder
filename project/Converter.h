#pragma once

#include "CImgWrapper.h"
#include "ImageRGB.h"

using namespace cimg_library;

class Converter
{
public:
	static void Convert(const CImgWrapper& iCImgWrapper, ImageRGB& oImage);
	static void Convert(const ImageRGB& iImageRGB, CImgWrapper& oCImgWrapper);
	static void Convert(const Channel& iChannel, CImgWrapper& oCImgWrapper);
};
