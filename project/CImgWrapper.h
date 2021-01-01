#pragma once

#include "CImg.h"
#include <string>


using namespace cimg_library;


class CImgWrapper
{
public:
	CImgWrapper() {}
	CImgWrapper(const std::string& iFilename) { Read(iFilename); }
	CImgWrapper(const CImg<unsigned char>& iCImg) { _im = iCImg; }

	size_t GetWidth() const { return _im.width(); }
	size_t GetHeight() const { return _im.height(); }

	void GetPixel(const int col, const int row, unsigned char& oRed, unsigned char& oGreen, unsigned char& oBlue) const;

	bool Read(const std::string& iFilename);

	void Save(const std::string& iFilename);
	void Show(const std::string& iTitle = "title", const int milliseconds = -1);

	void DrawLine(int x1, int y1, int x2, int y2, const unsigned char* color);

	bool AskUserAboutAColor(unsigned char& oR, unsigned char& oG, unsigned char& oB);

	void Resize();

private:
	CImg<unsigned char> _im;
};

