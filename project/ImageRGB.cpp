#include "ImageRGB.h"
#include "Converter.h"


std::vector<double> ImageRGB::BrightnessCoeff::Human = { 0.299, 0.587, 0.114 };
std::vector<double> ImageRGB::BrightnessCoeff::Mean  = { 0.333, 0.333, 0.333 };


ImageRGB::ImageRGB(const Channel& iR, const Channel& iG, const Channel& iB)
	: _r(iR)
	, _g(iG)
	, _b(iB) {
	_w = iR.GetWidth();
	_h = iR.GetHeight();
	// to do: check if other channels has the same sizes
}
double ImageRGB::GetBrightness(const double r, const double g, const double b, const Brightness& iBrightnessType) {
	std::vector<double> k(3);

	if (iBrightnessType == Brightness::Human)
		k = BrightnessCoeff::Human;
	if (iBrightnessType == Brightness::Mean)
		k = BrightnessCoeff::Mean;

	return k[0] * r + k[1] * g + k[2] * b;
}
Channel ImageRGB::GetBrightness(const Brightness& iBrightnessType) {

	//std::vector<std::vector<T> colors(_w)>(_h);
	Channel ch(_w, _h);

	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			
			double r = _r.Get(i, j);
			double g = _g.Get(i, j);
			double b = _b.Get(i, j);
			double br = GetBrightness(r, g, b, iBrightnessType);
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
bool ImageRGB::AskUserAboutAColor(double& oR, double& oG, double& oB) {
	CImgWrapper wrapper;
	Converter::Convert(*this, wrapper);
	
	unsigned char r, g, b;
	bool selected = wrapper.AskUserAboutAColor(r,g,b);
	if (!selected)
		return false;

	oR = static_cast<double> (r) / 255.0;
	oG = static_cast<double> (g) / 255.0;
	oB = static_cast<double> (b) / 255.0;

	return true;
}
