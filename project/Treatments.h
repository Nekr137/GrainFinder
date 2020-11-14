#pragma once

#include <iostream>
#include <windows.h>
#include "ImageRGB.h"
#include "CImgWrapper.h"
#include "Converter.h"
#include "Visualizer.h"
#include "Utils.h"
#include <assert.h>
#include "MathUtils.h"
#include <algorithm>
#include "Slic.h"

using namespace std;

void AskAboutColors(const Channel& iChannel, double& oBorderColors, double& oGrainColors) {

	std::cout << "\nInter a borders color:";
	oBorderColors = iChannel.AskUserAboutAColor(oBorderColors);
	std::cout << "\t" << static_cast<int>(oBorderColors * 255);

	std::cout << "\nInter a grains color:";
	oGrainColors = iChannel.AskUserAboutAColor(oGrainColors);
	std::cout << "\t" << static_cast<int>(oGrainColors * 255);
}


void treatment2(const std::string& iFile, const std::string& oFolder, const size_t iGrainsCnt, const double iCompactness) {

	CImgWrapper wr(iFile);
	ImageRGB rgb;
	Converter::Convert(wr, rgb);

	Channel& r = rgb.GetRedChannelRef();
	Channel& g = rgb.GetGreenChannelRef();
	Channel& b = rgb.GetBlueChannelRef();

	for (size_t i = 0; i < 1; ++i) {
		r = r.ApplyMask(Kernels::Gauss5x5);
		g = g.ApplyMask(Kernels::Gauss5x5);
		b = b.ApplyMask(Kernels::Gauss5x5);
	}
	r.Normalize();
	g.Normalize();
	b.Normalize();

	SLIC::Slic slic(&rgb);
	slic.Perform(iGrainsCnt, iCompactness);
	slic.Show();
}



void treatment4(const std::string& iFile, const std::string& oFolder) {

	Mask m;
	size_t w = 101, h = 101;
	m.second = 0.0;
	for (size_t j = 0; j < h; ++j) {
		std::vector<double> row;
		for (size_t i = 0; i < w; ++i) {
			if (i == w / 2 || j == h / 2 || i == j || i == h - j) {
				row.push_back(1.0);
				m.second += 1.0;
			}
			else
				row.push_back(0.0);
		}
		m.first.push_back(row);
	}

	auto loaded = Load(iFile);
	loaded.Save(oFolder + "01_loaded.jpg");

	loaded = loaded.ApplyMask(Kernels::Gauss5x5);
	loaded.Normalize();

	auto v = loaded.Copy();

	Channel im = loaded.ApplyMask(m);

	im.Normalize().Save(oFolder + "02_bigMask.jpg");
}

void treatment3(const std::string& iFile, const std::string& oFolder) {

	auto loaded = Load(iFile);
	loaded.Save(oFolder + "01_loaded.jpg");

	loaded = loaded.ApplyMask(Kernels::Gauss5x5);
	loaded.Normalize();

	Mask extV = { {
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		}, 9.0 };
	Mask extH = { {
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		}, 9.0 };
	Mask extA = { {
		{ 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 },
		{ 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0 },
		{ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
		{ 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0 },
		{ 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 },
		}, 33.0 };

	auto v = loaded.Copy();
	auto h = loaded.Copy();
	auto a = loaded.Copy();

	for (size_t i = 0; i < 100; ++i) {
		v = v.ApplyMask(extV);
		h = h.ApplyMask(extH);
		a = a.ApplyMask(extA);
	}

	(v + h).Normalize().Save(oFolder + "02_sum.jpg");
	a.Normalize().Save(oFolder + "03_all.jpg");
	v.Normalize().Save(oFolder + "03_extV.jpg");
	h.Normalize().Save(oFolder + "04_extH.jpg");

}


void treatment1(const std::string& iFile, const std::string& oFolder) {

	auto loaded = Load(iFile);

  loaded.Save(oFolder + "012.jpg");

  loaded = loaded.ApplyMask(Kernels::Gauss5x5);
  loaded.Normalize();

  loaded.RemoveOutlets(0.5).Save(oFolder + "02,outlets2.jpg");

  {
    Channel prew = loaded.Copy();
    prew =
      prew.ApplyMask(Kernels::SobelX).Abs() +
      prew.ApplyMask(Kernels::SobelY).Abs() +
      prew.ApplyMask(Kernels::NorthEast).Abs() +
      prew.ApplyMask(Kernels::NorthWest).Abs();

    prew.Normalize().Save(oFolder + "03 prew_norm2.jpg");

    prew.RemoveOutlets(0.2).Normalize().Save(oFolder + "04_remove_outplets2.jpg");

    Channel bm = prew.ApplyMask(Kernels::Gauss5x5).AsBitMap(prew.FindMedian() + 0.1*prew.FindSigma());
    bm.Save(oFolder + "07_bitmap2.jpg");

    Median(bm, 5).Save(oFolder + "08_median_toBm2.jpg");
  }
}
