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


void treatment1(const std::string& iFile, const std::string& oFolder) {
	static int treatmentIdx = 0;
	int outputIdx = 0;

	auto loaded = Load(iFile);

	auto distr = loaded.Distribution();
	Visualizer::Plot(distr, loaded);

  loaded.Show();
  loaded.Save(oFolder + "save.jpg");
}
