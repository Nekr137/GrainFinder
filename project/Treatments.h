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


void treatment1(const std::string& iFile) {
	static int treatmentIdx = 0;
	int outputIdx = 0;

	auto loaded = Load(iFile);
	loaded.Show();

	auto distr = loaded.Distribution();
	Visualizer::Plot(distr, loaded);

	loaded.Show();
}

void treatments() {
	std::vector<std::string> files = {
		"../tests/cutted.jpg",
		"../tests/1000and100/900_3_chas/4/x1000 krai.jpg",
		"../tests/1000and100/1000_3_chas/x1000 centr.jpg"
	};

	for (auto file : files) {
		treatment1(file);
	}
}