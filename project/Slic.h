#pragma once

#include <vector>
#include "ImageRGB.h"

namespace SLIC
{
	struct P2D {
		P2D(){};
		P2D(size_t iX, size_t iY) : x(iX), y(iY) {}
		size_t x = 0, y = 0;
	};
	struct P3D {
		P3D(){};
		P3D(double iX, double iY, double iZ) : x(iX), y(iY), z(iZ) {}
		double x = 0, y = 0, z = 0;
	};
	struct Cluster {
		Cluster(double l, double a, double b, double x, double y) {
			this->l = l; this->a = a; this->b = b; this->x = x; this->y = y;
			sx = static_cast<size_t>(std::round(x));
			sy = static_cast<size_t>(std::round(y));
		}
		std::vector<P2D> _aPixels;
		double x = 0.0, y = 0.0;
		double l = 0.0, a = 0.0, b = 0.0;
		size_t sx = 0, sy = 0;
	};


	class Slic {

	public:

		Slic(const ImageRGB* ipImage);
		bool Perform(const size_t K = 40, const double m = 10.0);
		void Show() const;

	private:
		double ClusterToPixelDist(const Cluster& iCl, const P2D& iPixel);
		double FindEuqlDist(const Cluster& iCl, const P2D& iPixel);
		double FindColorDist(const Cluster& iCl, const P2D& iPixel);
		void FindClusterBorders(const Cluster& iCLuster, size_t& oLeft, size_t& oRight, size_t& oTop, size_t& oBottom);

		std::vector<Cluster> _aClusters;
		std::vector<size_t> _pixelClusterIndices;
		std::vector<double> _pixelClusterBestDistances;
		const ImageRGB* _pImage;
		size_t _imgW = 0, _imgH = 0;
		size_t _S;
		double _m;
		size_t _countOfS = 2;
	};
};