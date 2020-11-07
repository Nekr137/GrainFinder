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

using namespace std;


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
