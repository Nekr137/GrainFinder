#include "ImageRGB.h"
#include "Converter.h"


ImageRGB::ImageRGB(const Channel& iR, const Channel& iG, const Channel& iB)
	: _r(iR)
	, _g(iG)
	, _b(iB) {
	_w = iR.GetWidth();
	_h = iR.GetHeight();
	// to do: check if other channels has the same sizes
}
Channel ImageRGB::GetBrightness() {

	//std::vector<std::vector<T> colors(_w)>(_h);
	Channel ch(_w, _h);

	const double k[] = { 0.299, 0.587, 0.114 };

	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			double r = k[0] * _r.Get(i, j);
			double g = k[1] * _g.Get(i, j);
			double b = k[2] * _b.Get(i, j);
			double br = r + g + b;
			ch.Set(i, j, br);
		}
	}
	return ch;
}
void ImageRGB::Save(const std::string& iFilename) {
	CImgWrapper wrapper;
	Converter::Convert(*this, wrapper);
	wrapper.Save(iFilename);
}