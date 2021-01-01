#pragma once

#include <vector>
#include "ImageRGB.h"
#include "Cluster.h"

namespace SLIC
{
	class Slic {

	public:

		Slic(const ImageRGB* ipImage);
		bool Perform(const size_t K = 40, const double m = 10.0);
		void Show() const;
		
		std::vector<Cluster>& GetClustersRef() { return _aClusters; }

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