#include "Utils.h"
#include "Visualizer.h"
#include "ImageRGB.h"
#include "CImgWrapper.h"
#include "Converter.h"
#include <iostream>

using namespace std;


Channel Load(const std::string& iFilename) {
	CImgWrapper wr(iFilename);

	ImageRGB rgb;
	Converter::Convert(wr, rgb);
	Channel res = rgb.GetBrightness();

	return res;
}
void VisualizeGradient(const Channel& iChannel, const std::string& iFilename) {
	Channel grad = Channel::GradientRectangle(256, 256);
	ImageRGB rect(grad, grad, grad);
		std::vector<double> distribution = iChannel.Distribution();
	Visualizer::Plot(distribution, rect);
	rect.Save(iFilename);
}
void ShowGradient(const Channel& iChannel) {
	Channel grad = Channel::GradientRectangle(256, 256);
	ImageRGB rect(grad, grad, grad);
	std::vector<double> distribution = iChannel.Distribution();
	Visualizer::Plot(distribution, rect);
	CImgWrapper w;
	Converter::Convert(rect, w);
	w.Show();
}


void Gauss5x5(Channel& iChannel, Channel& oGauss) {
	const Mask gauss5x5 = { {
		{ 1, 4, 7, 4, 1 },
		{ 4, 16, 26, 16, 4 },
		{ 7, 26, 41, 26, 7 },
		{ 4, 16, 26, 16, 4 },
		{ 1, 4, 7, 4, 1 }
		}, 273 };

	iChannel.ApplyMask(gauss5x5, oGauss);
}
void Prewitt(Channel& iChannel, Channel& oPrewittX, Channel& oPrewittY) {

	const Mask prewittKernelX = { { 
		{ -1.0, -1.0, -1.0 }, 
		{ 0.0, 0.0, 0.0 }, 
		{ 1.0, 1.0, 1.0 } 
		}, 1.0 };
	const Mask prewittKernelY = { { 
		{ -1.0, 0.0, 1.0 }, 
		{ -1.0, 0.0, 1.0 }, 
		{ -1.0, 0.0, 1.0 } 
		}, 1.0 };

	iChannel.ApplyMask(prewittKernelX, oPrewittX);
	iChannel.ApplyMask(prewittKernelY, oPrewittY);
}
void Sobel(Channel& iChannel, Channel& oSobelX, Channel& oSobelY) {

	const Mask sobelKernelX = { { 
		{ -1.0, -2.0, -1.0 }, 
		{ 0.0, 0.0, 0.0 }, 
		{ 1.0, 2.0, 1.0 } },
		1.0 };
	const Mask sobelKernelY = { { 
		{ -1.0, 0.0, 1.0 }, 
		{ -2.0, 0.0, 2.0 }, 
		{ -1.0, 0.0, 1.0 } 
		}, 1.0 };

	iChannel.ApplyMask(sobelKernelX, oSobelX);
	iChannel.ApplyMask(sobelKernelY, oSobelY);
}
Channel Median(Channel& iChannel, size_t iSize) {

	std::cout << "\nMedian filtering...";

	const size_t w = iChannel.GetWidth();
	const size_t h = iChannel.GetHeight();

	Channel res(w, h);

	for (size_t i = 0; i < w - iSize; ++i) {
		for (size_t j = 0; j < h - iSize; ++j) {

			std::vector<double> values;

			for (size_t mi = 0; mi < iSize; ++mi) {
				for (size_t mj = 0; mj < iSize; ++mj) {
					size_t x = i + mi;
					size_t y = j + mj;
					double value = iChannel.Get(x, y);
					values.push_back(value);
				}
			}
			std::sort(values.begin(), values.end());
			double median = values[iSize*iSize / 2];
			res.Set(i, j, median);
		}
	}
	return res;
}

Mask Kernels::Contrast = { {
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, -1, 0, 0 },
	{ 0, -1, 5, -1, 0 },
	{ 0, 0, -1, 0, 0 },
	{ 0, 0, 0, 0, 0 },
	}, 1 };

Mask Kernels::VerticalLines = { {
	{ -1, 2, -1 }, // fixed (changed from -1 0 -1)
	{ -1, 2, -1 },
	{ -1, 2, -1 }
	}, 1 };

Mask Kernels::HorizontalLines = { {
	{ -1, -1, -1 },
	{ 2,2,2 },
	{ -1, -1, -1 },
	}, 1 };

Mask Kernels::Gauss5x5 = { {
	{ 1, 4, 7, 4, 1 },
	{ 4, 16, 26, 16, 4 },
	{ 7, 26, 41, 26, 7 },
	{ 4, 16, 26, 16, 4 },
	{ 1, 4, 7, 4, 1 }
	}, 273 };


