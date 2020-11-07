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
Channel ImageRGB::GetBrightness(const Brightness& iBrightnessType) {

	//std::vector<std::vector<T> colors(_w)>(_h);
	Channel ch(_w, _h);

	std::vector<double> k(3);

	if (iBrightnessType == Brightness::Human)
		k = { 0.299, 0.587, 0.114 };
	if (iBrightnessType == Brightness::Mean)
		k = { 0.333, 0.333, 0.333 };

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
void ImageRGB::Show(const std::string& iTitle) {
	CImgWrapper wrapper;
	Converter::Convert(*this, wrapper);
	wrapper.Show(iTitle);
}