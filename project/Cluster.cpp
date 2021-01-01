#include "Cluster.h"
#include <algorithm>


Cluster::Cluster(double l, double a, double b, double x, double y, const ImageRGB* ipImg) {
	this->l = l; this->a = a; this->b = b; this->x = x; this->y = y;
	sx = static_cast<size_t>(std::round(x));
	sy = static_cast<size_t>(std::round(y));
	_pImg = ipImg;
}

void Cluster::FindClusterColorParams(double& oMean, double& oSigma) const {

	double sum = 0.0;

	size_t n = _aPixels.size();
	std::vector<double> aBr(n);
	for (size_t i = 0; i < n; ++i) {
		double r, g, b;
		P2D p = _aPixels[i];
		_pImg->Get(p.x, p.y, r, g, b);
		double br = (r + g + b) / 3.0;
		aBr[i] = br;
		sum += br;
	}

	double nd = static_cast<double>(n);
	oMean = sum / nd;

	oSigma = 0.0;
	for (double br : aBr) {
		double dif = br - oMean;
		oSigma += dif*dif;
	}

	oSigma = std::sqrt(oSigma / n);
}

