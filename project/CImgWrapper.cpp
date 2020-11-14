#include "CImgWrapper.h"

#include <string>
#include <vector>

#include "CImg.h"
#include <iostream>
#include "Converter.h"
#include <assert.h>

using namespace cimg_library;


bool CImgWrapper::Read(const std::string& iFilename) {

	std::cout << "\nReading an image via CImg...";
	_im = CImg<unsigned char>(iFilename.c_str());
	return true;
}

void CImgWrapper::GetPixel(const int col, const int row, unsigned char& oRed, unsigned char& oGreen, unsigned char& oBlue) const {

	assert(static_cast<int>(col) < _im.width() && static_cast<int>(row) < _im.height());

	oRed =   _im(col, row, 0, 0);
	oGreen = _im(col, row, 0, 1);
	oBlue =  _im(col, row, 0, 2);
}

void CImgWrapper::Save(const std::string& iFilename) {

	std::cout << "\nSaving an image into " << iFilename << "...";
	_im.save(iFilename.c_str());
}

void CImgWrapper::Show(const std::string& iTitle) {

	std::cout << "\nShowing an image...";
	CImgDisplay main_disp(_im, iTitle.c_str());

	const int w = _im.width();
	const int h = _im.height();

	while (!main_disp.is_closed())
	{
		main_disp.wait();

		if (main_disp.button() && main_disp.mouse_y() >= 0 && main_disp.mouse_x() >= 0) {

			const int x = main_disp.mouse_x() * w / main_disp.width();
			const int y = main_disp.mouse_y()* h / main_disp.height();

			unsigned char r, g, b;
			GetPixel(x, y, r, g, b);

			unsigned char cc[] = { 255, 0, 0 };
			_im.draw_point(x, y, cc, 1).display(main_disp);

			std::cout << "\nCoo: (" << x << ", " << y << "), color: (" << (int)r << "," << (int)g << "," << (int)b << ")" << std::endl;
		}
	}
}

bool CImgWrapper::AskUserAboutAColor(unsigned char& oR, unsigned char& oG, unsigned char& oB) {

	CImgDisplay main_disp(_im, "Choose a color");

	const int w = _im.width();
	const int h = _im.height();

	while (!main_disp.is_closed())
	{
		main_disp.wait();

		if (main_disp.button() && main_disp.mouse_y() >= 0) {

			const int x = main_disp.mouse_x() * w / main_disp.width();
			const int y = main_disp.mouse_y() * h / main_disp.height();

			unsigned char r, g, b;
			GetPixel(x, y, r, g, b);

			std::cout << "\nCoo: (" << x << ", " << y << "), color: (" << (int)r << "," << (int)g << "," << (int)b << ")" << std::endl;

			oR = r;
			oG = g;
			oB = b;

			return true; // color was selected
		}
	}
	return false; // color was not selected
}

void CImgWrapper::DrawLine(int x1, int y1, int x2, int y2, const unsigned char* color) {
	_im.draw_line(x1, y1, x2, y2, color);
}

void Resize() {

}