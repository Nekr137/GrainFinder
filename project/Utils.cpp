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
	Channel res = rgb.GetBrightness(ImageRGB::Brightness::Human);

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
	{ 0.0, 0.0, 0.0, 0.0, 0.0 },
	{ 0.0, 0.0,-1.0, 0.0, 0.0 },
	{ 0.0,-1.0, 5.0,-1.0, 0.0 },
	{ 0.0, 0.0,-1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0, 0.0, 0.0 },
	}, 1.0 };

Mask Kernels::VerticalLines = { {
	{ -1.0, 2.0, -1.0 }, // fixed (changed from -1 0 -1)
	{ -1.0, 2.0, -1.0 },
	{ -1.0, 2.0, -1.0 }
	}, 1.0 };

Mask Kernels::HorizontalLines = { {
	{-1.0,-1.0,-1.0 },
	{ 2.0, 2.0, 2.0 },
	{-1.0,-1.0,-1.0 },
	}, 1.0 };

Mask Kernels::Gauss5x5 = { {
	{ 1.0, 4.0, 7.0, 4.0, 1.0 },
	{ 4.0, 16.0, 26.0, 16.0, 4.0 },
	{ 7.0, 26.0, 41.0, 26.0, 7 },
	{ 4.0, 16.0, 26.0, 16.0, 4.0 },
	{ 1.0, 4.0, 7.0, 4.0, 1.0 }
	}, 273.0 };

Mask Kernels::PrewittX = { {
	{ -1.0, -1.0, -1.0 },
	{ 0.0, 0.0, 0.0 },
	{ 1.0, 1.0, 1.0 }
	}, 1.0 };

Mask Kernels::PrewittY = { {
	{ -1.0, 0.0, 1.0 },
	{ -1.0, 0.0, 1.0 },
	{ -1.0, 0.0, 1.0 }
	}, 1.0 };

Mask Kernels::SobelX = { {
	{-1.0,-2.0,-1.0 },
	{ 0.0, 0.0, 0.0 },
	{ 1.0, 2.0, 1.0 } 
	}, 1.0 };

Mask Kernels::SobelY = { {
	{ -1.0, 0.0, 1.0 },
	{ -2.0, 0.0, 2.0 },
	{ -1.0, 0.0, 1.0 }
	}, 1.0 };
