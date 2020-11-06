#include "CImgWrapper.h"

#include <string>
#include <vector>

#include "CImg.h"
#include <iostream>
#include "Converter.h"

using namespace cimg_library;


bool CImgWrapper::Read(const std::string& iFilename) {

	std::cout << "\nReading an image via CImg...";
	_im = CImg<unsigned char>(iFilename.c_str());
	return true;
}

void CImgWrapper::GetPixel(size_t col, size_t row, unsigned char& oRed, unsigned char& oGreen, unsigned char& oBlue) const {
	oRed =   (int)_im(col, row, 0, 0);
	oGreen = (int)_im(col, row, 0, 1);
	oBlue =  (int)_im(col, row, 0, 2);
}

void CImgWrapper::Save(const std::string& iFilename) {

	std::cout << "\nSaving an image into " << iFilename << "...";
	_im.save(iFilename.c_str());
}

void CImgWrapper::Show(const std::string& iTitle) {

	std::cout << "\nShowing an image...";
	CImgDisplay main_disp(_im, iTitle.c_str());
	while (!main_disp.is_closed())
	{
		main_disp.wait();

		if (main_disp.button() && main_disp.mouse_y() >= 0) {

			const int x = main_disp.mouse_x();
			const int y = main_disp.mouse_y();

			unsigned char r, g, b;
			GetPixel(x, y, r, g, b);

			std::cout << "\nCoo: (" << x << ", " << y << "), color: (" << (int)r << "," << (int)g << "," << (int)b << ")" << std::endl;
		}
	}
}

void CImgWrapper::DrawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, const unsigned char* color) {
	_im.draw_line(
		static_cast<unsigned char> (x1),
		static_cast<unsigned char> (y1),
		static_cast<unsigned char> (x2),
		static_cast<unsigned char> (y2),
		color);
}