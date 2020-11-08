#include "Converter.h"
#include <iostream>
#include "Channel.h"
#include <assert.h>


void Converter::Convert(const CImgWrapper& iCImgWrapper, ImageRGB& oImage)  {

	size_t w = iCImgWrapper.GetWidth();
	size_t h = iCImgWrapper.GetHeight();

	Channel ch[3] = { Channel(w, h), Channel(w, h), Channel(w, h) };

	for (size_t i = 0; i < w; ++i) {
		for (size_t j = 0; j < h; ++j) {
			unsigned char r, g, b;
			iCImgWrapper.GetPixel(i, j, r, g, b);
			ch[0].Set(i, j, static_cast<double>(r) / 255.0);
			ch[1].Set(i, j, static_cast<double>(g) / 255.0);
			ch[2].Set(i, j, static_cast<double>(b) / 255.0);
		}
	}

	oImage = ImageRGB(ch[0], ch[1], ch[2]);
}

void Converter::Convert(const ImageRGB& iImageRGB, CImgWrapper& oCImgWrapper) {

	size_t w = iImageRGB.GetWidth();
	size_t h = iImageRGB.GetHeight();

	CImg<unsigned char> im(w, h, 1, 3, 0);

	Channel r = iImageRGB.GetRedChannelRef();
	Channel g = iImageRGB.GetGreenChannelRef();
	Channel b = iImageRGB.GetBlueChannelRef();

	for (size_t i = 0; i < w; ++i) {
		for (size_t j = 0; j < h; ++j) {

			unsigned char color3[3] = {
				static_cast<unsigned char>(r.Get(i, j) * 255.0),
				static_cast<unsigned char>(g.Get(i, j) * 255.0),
				static_cast<unsigned char>(b.Get(i, j) * 255.0)
			};

			im.draw_point(i, j, color3);
		}
	}
	oCImgWrapper = CImgWrapper(im);
}

void Converter::Convert(const Channel& iChannel, CImgWrapper& oCImgWrapper) {

	size_t w = iChannel.GetWidth();
	size_t h = iChannel.GetHeight();

	CImg<unsigned char> im(w, h, 1, 3, 0);

	for (size_t i = 0; i < w; ++i) {
		for (size_t j = 0; j < h; ++j) {
			double color = iChannel.Get(i, j);

			assert(color > 0.0 - 1e-12);
			assert(color < 1.0 + 1e-12);
			unsigned char c = static_cast<unsigned char>(color * 255.0);

			unsigned char color3[3] = { c, c, c };
			im.draw_point(i, j, color3);
		}
	}
	oCImgWrapper = CImgWrapper(im);
}
