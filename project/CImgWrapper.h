#pragma once

#include "CImg.h"
#include <string>


using namespace cimg_library;


class CImgWrapper
{
public:
	CImgWrapper() {}
	CImgWrapper(const std::string& iFilename) { Read(iFilename); }
	CImgWrapper(const CImg<int>& iCImg) { _im = iCImg; }

	size_t GetWidth() const { return _im.width(); }
	size_t GetHeight() const { return _im.height(); }

	void GetPixel(size_t col, size_t row, unsigned char& oRed, unsigned char& oGreen, unsigned char& oBlue) const;

	bool Read(const std::string& iFilename);

	void Save(const std::string& iFilename);
	void Show(const std::string& iTitle = "title");

	void DrawLine(int x1, int y1, int x2, int y2, const unsigned char* color);

private:
	CImg<unsigned char> _im;
};

