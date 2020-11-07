#pragma once

#include <iostream>
#include <windows.h>
#include "ImageRGB.h"
#include "CImgWrapper.h"
#include "Converter.h"
#include "Visualizer.h"
#include "Utils.h"
#include <assert.h>

using namespace std;


bool IsEqual(const double iV1, const double iV2, const double iTol = 1e-12) {
	return std::abs(iV2 - iV1) < iTol;
}

void unit_test_normalize() {
	{
		Channel ch(3, 3);
		ch.Set(0, 0, -3.0); ch.Set(1, 1, -2.0); ch.Set(2, 0, -1.0);
		ch.Set(0, 1, 0.0); ch.Set(1, 1, 1.0); ch.Set(2, 1, 2.0);
		ch.Set(0, 2, 3.0); ch.Set(1, 2, 4.0); ch.Set(2, 2, 5.0);
		ch.Normalize();
		for (size_t i = 0; i < 3; ++i) {
			for (size_t j = 0; j < 3; ++j) {
				assert(ch.Get(i, j) < 1.0 + 1e-12);
				assert(ch.Get(i, j) > -1e-12);
			}
		}
	}
	{
		Channel ch(2, 2);
		ch.Set(0, 0, -2.0); ch.Set(0, 1, +2.0);
		ch.Normalize();
		assert(IsEqual(ch.Get(0, 0), 0.0));
		assert(IsEqual(ch.Get(0, 1), 1.0));
		assert(IsEqual(ch.Get(1, 0), 0.5));
		assert(IsEqual(ch.Get(1, 1), 0.5));
	}
}
void unit_test_vertical_edge() {
	const size_t w = 10;
	const size_t h = 6;
	Channel ch(w, h);
	for (size_t i = w / 2; i < w; ++i){
		for (size_t j = 0; j < h; ++j) {
			ch.Set(i, j, 1.0);
		}
	}

	std::cout << "\nOriginal:\n";
	ch.Print();

	std::cout << "\nPrewittX:\n";
	ch.ApplyMask(Kernels::PrewittX).Print();

	std::cout << "\nSobelX:\n";
	ch.ApplyMask(Kernels::SobelX).Print();

	std::cout << "\nGauss5x5:\n";
	ch.ApplyMask(Kernels::Gauss5x5).Print();
}

void unit_test_distribution() {
	{
		Channel ch(2, 2);
		ch.Set(0, 0, 0.0); ch.Set(0, 1, 0.25);
		ch.Set(1, 0, 0.5); ch.Set(1, 1, 0.5);

		std::vector<double> distr = ch.Distribution();

		Channel grad = Channel::GradientRectangle(256, 256);
		ImageRGB rect(grad, grad, grad);
		Visualizer::Plot(distr, rect);
		//rect.Show("distr_unit_test");
	}
	{
		Channel ch(256, 256);
		const size_t a = 128;
		for (size_t i = 0; i < a; ++i)
			for (size_t j = 0; j < a; ++j)
				ch.Set(i, j, 1.0);

		ch = ch.ApplyMask(Kernels::Gauss5x5);

		Channel smoothed;
		ch.Crop(2, 2, 2, 2, smoothed);
		smoothed.Normalize();

		std::vector<double> distr = smoothed.Distribution();
		ImageRGB rect(smoothed, smoothed, smoothed);
		Visualizer::Plot(distr, rect);
		//rect.Show("distr_unit_test__smoothing");
	}
}

void unit_test_crop() {
	{
		Channel ch(2, 2);
		ch.Set(0, 0, 1); ch.Set(1, 0, 2);
		ch.Set(0, 1, 3); ch.Set(1, 1, 4);

		Channel crop;
		ch.Crop(1, 0, 1, 0, crop);

		assert(ch.Get(1, 1) == crop.Get(0, 0));
		assert(crop.GetWidth()  == 1);
		assert(crop.GetHeight() == 1);
	}
	{
		Channel ch(2, 2);
		ch.Set(0, 0, 1); ch.Set(1, 0, 2);
		ch.Set(0, 1, 3); ch.Set(1, 1, 4);

		Channel crop;
		ch.Crop(0, 1, 0, 1, crop);

		assert(ch.Get(0, 0) == crop.Get(0, 0));
		assert(crop.GetWidth() == 1);
		assert(crop.GetHeight() == 1);
	}
}

void unit_tests() {
	unit_test_distribution();
	unit_test_crop();
	unit_test_vertical_edge();
	unit_test_normalize();
}

void testVertLines() {
	auto original = Load("../tests/1x1000B.jpg");
	original.Save("../output/01_original.jpg");
	original.ApplyMask(Kernels::Gauss5x5);
	Channel ch1, ch2;
	ch1 = original.Copy();
	ch2 = original.Copy();

	Channel v = ch1.Normalize()
		.ApplyMask(Kernels::VerticalLines)
		.ApplyMask(Kernels::Gauss5x5);
	v.Normalize().Save("../output/02_vert.jpg");

	Median(v, 3).Save("../output/02_vert_median.jpg");
	
	ch2.Normalize()
		.ApplyMask(Kernels::HorizontalLines)
		.ApplyMask(Kernels::Gauss5x5)
		.Save("../output/03_hor.jpg");
}

void testContrast() {

	auto original = Load("tests/1x1000B.jpg");
	original.Save("output/01_original.jpg");
	original.ApplyMask(Kernels::Contrast).Save("output/02_contrast.jpg");
}

void testHorSmall() {

	Mask m = {{
		{ 1, 1, 1 },
		{ 10, 10, 10 },
		{ 1, 1, 1 }
		}, 16 };

	auto original = Load("tests/1x1000B.jpg");
	original.Save("output/01_original.jpg");

	original.Invert().Save("output/02_invert.jpg");

	Channel ch;
	original.ApplyMask(m, ch);
	ch.Save("output/03_hor.jpg");
}

static Mask CreateLineMask() {
	const size_t n = 30;
	std::vector<std::vector<double>> m = std::vector<std::vector<double>>(n, std::vector<double>(n));
	double cnt = 0.0;
	for (size_t i = n/3; i < n/3*2; ++i) {
		for (size_t j = 0; j < n; ++j) {
			m[i][j] = 1.0;
			cnt += 1.0;
		}
	}
	Mask mask = { m, cnt };
	return mask;
}

void testBigLine() {
	auto m = CreateLineMask();

	auto original = Load("tests/1x1000B.jpg");
	original.Save("output/01_original.jpg");

	original.Invert().Save("output/02_invert.jpg");

	Channel horLine;
	original.ApplyMask(m, horLine);
	
	//horLine.Abs().Save("output/03_hor_line_big.jpg");
	horLine.Normalize().Save("output/03_hor_line_big.jpg");
}

void test45Line() {

	const Mask line45_5x5 = { {
		{ -25.0, -16.0, 0.0, 16.0, 25.0 },
		{ -16.0, 0.0, 16.0, 25.0, 16.0 },
		{ 0.0, 16.0, 25.0, 16.0, 0.0 },
		{ 16.0, 25.0, 16.0, 0.0, -16.0 },
		{ 25.0, 16.0, 0.0, -16.0, -25.0 },
		}, 1.0 };
	const Mask line135_5x5 = { {
		{ 25.0, 16.0, 0.0, -16.0, -25.0 },
		{ 16.0, 25.0, 16.0, 0.0, -16.0 },
		{ 0.0, 16.0, 25.0, 16.0, 0.0 },
		{ -16.0, 0.0, 16.0, 25.0, 16.0 },
		{ -25.0, -16.0, 0.0, 16.0, 25.0 },
		}, 1.0 };

	auto original = Load("tests/1x1000B.jpg");
	original.Save("output/01_original.jpg");

	original.Invert().Save("output/02_invert.jpg");

	Channel lines45;
	original.ApplyMask(line45_5x5, lines45);
	//lines45.Abs().Save("output/02_45.jpg");
	lines45.Normalize().Save("output/02_45.jpg");

	Channel lines135;
	original.ApplyMask(line135_5x5, lines135);
	//lines135.Abs().Save("output/02_135.jpg");
	lines135.Normalize().Save("output/02_135.jpg");
}

void testBitMap() {
	Channel ch(2, 2);
	ch.Set(0, 0, 0.25);
	ch.Set(1, 0, 0.5);
	ch.Set(0, 1, 0.75);
	ch.Set(1, 1, 1.0);

	ch = ch.AsBitMap(135.0 / 255.0);	
}

void test2() {
	Channel ch(2, 2);
	ch.Set(0, 0, 0.0);
	ch.Set(1, 0, 0.5);
	ch.Set(0, 1, 0.5);
	ch.Set(1, 1, 1.0);

	ch.Pow(2.0);
	ch = ch + ch;
	ch.Normalize();
}

