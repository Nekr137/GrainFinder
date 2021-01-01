#include "ClusterProcessor.h"
#include <algorithm>
#include <assert.h>
#include <iostream>


static void ColorGenerator(double& oR, double& oG, double& oB) {
	static int idx = 0;
	oR = 0.0; 
	oG = 0.0;
	oB = 0.0;

	switch (idx) {
	case 0:
		oR = 1.0;
		break;
	case 1:
		oG = 1.0;
		break;
	case 2:
		oB = 1.0;
		break;
	case 3:
		oR = 1.0; oB = 1.0;
		break;
	case 4:
		oR = 1.0; oG = 1.0;
		break;
	case 5:
		oG = 1.0; oB = 1.0;
		break;
	default:
		oR = 1.0; oG = 1.0; oB = 1.0;
	}
	idx++;
	if (idx == 7)
		idx = 0;
}

void ClusterProcessor::Perform() {

	// Created a canvas
	Channel ch(_imgW * 3, _imgH * 3);
	ImageRGB debugIm(ch, ch, ch);

	// Show clusters
	for (const Cluster& cl : _aClusters) {
		double r = 0.0, g = 0.0, b = 0.0;
		ColorGenerator(r, g, b);
		for (const P2D& p : cl._aPixels)
			debugIm.Set(p.X(), p.Y(), r, g, b);
	}

	// Split each cluster into separate parts
	SplitClusters();

	// Show separated clusters
	for (const Cluster& cl : _aClusters) {
		double r = 0.0, g = 0.0, b = 0.0;
		ColorGenerator(r, g, b);
		for (const P2D& p : cl._aPixels) {
			debugIm.Set(p.X() + _imgW, p.Y(), r, g, b);
		}
		//debugIm.Show("ttt", 1500);
	}

	// ---
	std::vector<Border> aBorders;
	FindBorders(aBorders);

	// Show borders
	for (const Border& border : aBorders) {
		double r = 0.0, g = 0.0, b = 0.0;
		ColorGenerator(r, g, b);
		for (const BorderPnt& p : border.GetPointsRef()) {
			auto pos = p._pos;
			debugIm.Set(pos.X(), pos.Y() + _imgH, r, g, b);
		}
	}
	debugIm.Show();

	
	// ---
	RemoveInnerClusters(aBorders);


	// Show borders
	for (const Border& border : aBorders) {
		double r = 0.0, g = 0.0, b = 0.0;
		ColorGenerator(r, g, b);
		for (const BorderPnt& p : border.GetPointsRef()) {
			auto pos = p._pos;
			debugIm.Set(pos.X() + _imgW, pos.Y() + _imgH, r, g, b);
		}
	}
	debugIm.Show();
}

void ClusterProcessor::RemoveInnerClusters(std::vector<Border>& ioaBorders) {

	std::cout << "\n [ Cluster processor ] Removing inner clusters";

	size_t nClusters = _aClusters.size();
	std::vector<std::vector<Border>> aBordersByCluster(nClusters);

	for (const Border& b : ioaBorders) {

		int cl1 = b.GetCluster1Idx();
		int cl2 = b.GetCluster2Idx();

		if (cl1 >= 0)
			aBordersByCluster[cl1].push_back(b);
		if (cl2 >= 0)
			aBordersByCluster[cl2].push_back(b);
	}

	for (size_t i = nClusters; i > 0; --i) {
		if (aBordersByCluster[i - 1].size() < 2) {
			_aClusters.erase(_aClusters.begin() + i - 1);
			aBordersByCluster.erase(aBordersByCluster.begin() + i - 1);
		}
	}

	ioaBorders.clear();

	for (const std::vector<Border>& clusterBorders : aBordersByCluster) {
		for (const Border& border : clusterBorders) {
			ioaBorders.push_back(border);
		}
	}
}

void ClusterProcessor::UniteSameByColor(const std::vector<Cluster>& iaClusters, std::vector<Cluster>& oaClusters) {

	oaClusters = iaClusters;

	size_t n = oaClusters.size();
	for (size_t i = n-1; i > 0; --i) {

		Cluster& cl_i = oaClusters[i];
		double mean_i, sigma_i;
		cl_i.FindClusterColorParams(mean_i, sigma_i);

		for (size_t j = i-1; j > 0; --j) {
			if (i == j)
				continue;
		
			Cluster& cl_j = oaClusters[j];
			double mean_j, sigma_j;
			cl_j.FindClusterColorParams(mean_j, sigma_j);

			if (std::abs(mean_j - mean_i) < 1.0 * sigma_i) {

				cl_j._aPixels.insert(cl_j._aPixels.end(), cl_i._aPixels.begin(), cl_i._aPixels.end());
				cl_i._aPixels.clear();
				break;
			}
		}
	}
}

void ClusterProcessor::FindBorders(std::vector<Border>& oaBorders) {

	std::cout << "\n [ Cluster processor ] Find borders";

	oaBorders.clear();

	// 2D array of cluster indices _imgW x _imgH
	std::vector<std::vector<size_t>> aaCI(_imgW, std::vector<size_t>(_imgH));
	int nClusters = _aClusters.size();

	for (int clIdx = 0; clIdx < nClusters; ++clIdx) {
		const Cluster& cl = _aClusters[clIdx];
		for (int i = 0; i < static_cast<int>(cl._aPixels.size()); ++i) {
			const P2D px = cl._aPixels[i];
			aaCI[px.x][px.y] = clIdx;
		}
	}

	// Find all border points comparing cluster indices
	std::vector<BorderPnt> aBorderPnts;
	for (size_t i = 0; i < _imgW; ++i) {
		for (size_t j = 0; j < _imgH; ++j) {

			P2D pos(i, j);

			size_t currClusterIdx = aaCI[i][j];

			if (i == 0) {
				BorderPnt bp = { pos, PicBorderIdx::Left, currClusterIdx};
				aBorderPnts.push_back(bp);
				continue;
			}
			if (j == 0) {
				BorderPnt bp = { pos, PicBorderIdx::Top, currClusterIdx };
				aBorderPnts.push_back(bp);
				continue;
			}

			size_t leftClusterIdx = aaCI[i-1][j];
			size_t upperClusterIdx = aaCI[i][j-1];

			if (currClusterIdx != leftClusterIdx) {
				BorderPnt bp = { pos, currClusterIdx, leftClusterIdx };
				aBorderPnts.push_back(bp);
			}
			else if (currClusterIdx != upperClusterIdx) {
				BorderPnt bp = { pos, currClusterIdx, upperClusterIdx };
				aBorderPnts.push_back(bp);
			}
			else if (i + 1 == _imgW) {
				BorderPnt bp = { pos, PicBorderIdx::Right, currClusterIdx };
				aBorderPnts.push_back(bp);
				continue;
			}
			else if (j + 1 == _imgH) {
				BorderPnt bp = { pos, PicBorderIdx::Bottom, currClusterIdx };
				aBorderPnts.push_back(bp);
				continue;
			}
		}
	}

	std::vector<Border> aBorders;
	for (int clIdx1 = -4; clIdx1 < nClusters; ++clIdx1) {
		std::cout << ".";
		for (int clIdx2 = clIdx1 + 1; clIdx2 < nClusters; ++clIdx2) {

			std::vector<BorderPnt> thisClustersBorderPnts;
			for (size_t i = aBorderPnts.size(); i > 0; --i) {
				const BorderPnt& p = aBorderPnts[i-1];
				if ((p._cl1Idx == clIdx1 && p._cl2Idx == clIdx2) || (p._cl1Idx == clIdx2 && p._cl2Idx == clIdx1)) {
					thisClustersBorderPnts.push_back(p);
					aBorderPnts.erase(aBorderPnts.begin() + i - 1);
				}
			}
			if (!thisClustersBorderPnts.empty())
				oaBorders.push_back(Border(thisClustersBorderPnts, clIdx1, clIdx2));
		}
	}
}
void ClusterProcessor::SplitClusters() {
	std::cout << "\n [ Cluster processor ] Cluster separation";
	std::vector<Cluster> aSplittedClusters;
	for (const Cluster& cl : _aClusters) {
		std::vector<Cluster> splitted;
		Split(cl, splitted);
		aSplittedClusters.insert(aSplittedClusters.end(), std::make_move_iterator(splitted.begin()), std::make_move_iterator(splitted.end()));
	}
	_aClusters = std::move(aSplittedClusters);

}
void ClusterProcessor::Split(const Cluster& iCluster, std::vector<Cluster>& oaClusters) {

	oaClusters.clear();

	const ImageRGB* pImg = iCluster._pImg;
	const size_t nPixels = iCluster._aPixels.size();

	std::vector<std::vector<P2D>> aClusters(nPixels);
	std::vector<std::vector<P2D>*> apClusters(nPixels);

	for (size_t i = 0; i < nPixels; ++i) {
		const P2D& pnt = iCluster._aPixels[i];
		aClusters[i] = { pnt };
		apClusters[i] = &aClusters[i];
	}

	auto isConnected = [](const P2D& iP1, const P2D& iP2) -> bool {

		double dx = iP2.X() - iP1.X();
		double dy = iP2.Y() - iP1.Y();
		double dist = dx*dx + dy*dy;
		return dist < 2.01;
	};
	auto isClustersConnected = [&isConnected](const std::vector<P2D>* ipCl1, const std::vector<P2D>* ipCl2) -> bool {
		for (size_t i = 0; i < ipCl1->size(); ++i) {
			for (size_t j = 0; j < ipCl2->size(); ++j) {
				if (isConnected(ipCl1->at(i), ipCl2->at(j))) {
					return true;
				}
			}
		}
		return false;
	};

	bool finished = false;
	for (size_t k = 0; !finished; ++k) {
		assert(k < 100000);
		finished = true;
		for (size_t i = 0; i + 1 < nPixels; ++i) {
			auto& pCl1 = apClusters[i];
			if (pCl1 == nullptr)
				continue;
			for (size_t j = i + 1; j < nPixels; ++j) {
				auto& pCl2 = apClusters[j];
				if (pCl2 == nullptr)
					continue;
				if (isClustersConnected(pCl1, pCl2)) {
					//pCl1->insert(pCl1->end(), std::make_move_iterator(pCl2->begin()), std::make_move_iterator(pCl2->end()));
					for (const P2D& p : (*pCl2))
						pCl1->push_back(p);
					pCl2 = nullptr;
					finished = false;
				}
			}
		}
	}

	for (const auto& pCl : apClusters) {
		if (!pCl)
			continue;

		double x = 0.0, y = 0.0;
		for (const P2D& p : (*pCl)) {
			x += p.X();
			y += p.Y();
		}
		x /= pCl->size();
		y /= pCl->size();

		Cluster cl(iCluster.l, iCluster.a, iCluster.b, x, y, iCluster._pImg);
		cl._aPixels = *pCl;
		oaClusters.push_back(cl);
	}
}