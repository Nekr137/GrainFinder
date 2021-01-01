#pragma once
#include <vector>
#include "ImageRGB.h"
#include "P2D.h"
#include "Region.h"

class Cluster
{
public:
	Cluster(double l, double a, double b, double x, double y, const ImageRGB* ipImg);

	void FindClusterColorParams(double& oMean, double& oSigma) const;

	const ImageRGB* _pImg = nullptr;
	std::vector<P2D> _aPixels;
	double x = 0.0, y = 0.0;
	double l = 0.0, a = 0.0, b = 0.0;
	size_t sx = 0, sy = 0;
};

