#include "Visualizer.h"
#include <assert.h>
#include <algorithm>
#include "Converter.h"
#include <iostream>

void Visualizer::Plot(const std::vector<double>& iY, Channel& ioCanvas) {
	ImageRGB imageRGB(ioCanvas, ioCanvas, ioCanvas);
	Plot(iY, imageRGB);
	ioCanvas = imageRGB.GetRedChannelRef().Copy();
}

void Visualizer::Plot(const std::vector<double>& iY, ImageRGB& ioCanvas) {
	int n = iY.size();
	std::vector<double> xData(n);
	for (int i = 0; i < n; ++i)
		xData[i] = static_cast<double>(i);
	Plot(xData, iY, ioCanvas);
}

void Visualizer::Plot(const std::vector<double>& iX, const std::vector<double>& iY, ImageRGB& ioCanvas) {

	size_t n = iX.size();
	assert(iY.size() == n);

	const unsigned char color[] = { 255, 0, 0 };

	double s[2] = {
		static_cast<double>(ioCanvas.GetWidth() - 1),
		static_cast<double>(ioCanvas.GetHeight() - 1)
	};

	assert(ioCanvas.GetWidth() >= n);

	double ma[2] = {
		*std::max_element(iX.begin(), iX.end()),
		*std::max_element(iY.begin(), iY.end())
	};
	double k[2] = { s[0] / ma[0], s[1] / ma[1] };

	CImgWrapper wrapper;
	Converter::Convert(ioCanvas, wrapper);
	for (size_t i = 1; i < n; ++i) {

		double x1 = k[0] * iX[i - 1];
		double y1 = s[1] - k[1] * iY[i - 1];
		double x2 = k[0] * iX[i];
		double y2 = s[1] - k[1] * iY[i];

		wrapper.DrawLine(
			static_cast<int> (x1),
			static_cast<int> (y1),
			static_cast<int> (x2),
			static_cast<int> (y2),
			color);
	}
	Converter::Convert(wrapper, ioCanvas);
}
