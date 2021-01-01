#pragma once

#include <vector>
#include "Cluster.h"

struct BorderPnt;
class ClusterProcessor;
class Border;

struct BorderPnt
{
	P2D _pos;
	size_t _cl1Idx, _cl2Idx;
};

enum PicBorderIdx
{
	Left =- 1, Right =- 2, Top =- 3, Bottom =- 4
};

class Border
{
public:
	Border() {}
	Border(const std::vector<BorderPnt>& iaPnts, const int iClusterIdx1, const int iClusterIdx2)
		: _aPnts(iaPnts)
		, _cl1Idx(iClusterIdx1)
		, _cl2Idx(iClusterIdx2) {}

	const std::vector<BorderPnt>& GetPointsRef() const { return _aPnts; }
	int GetCluster1Idx() const { return _cl1Idx; }
	int GetCluster2Idx() const { return _cl2Idx; }

private:
	std::vector<BorderPnt> _aPnts;
	int _cl1Idx, _cl2Idx;
};

class ClusterProcessor
{
public:
	ClusterProcessor(const std::vector<Cluster>& iaClusters) : _aClusters(iaClusters) {
		const auto img = _aClusters.front()._pImg;
		_imgW = img->GetWidth();
		_imgH = img->GetHeight();
	}

	void Perform();

	// Old method. Borders are not taken into account
	void UniteSameByColor(const std::vector<Cluster>& iaClusters, std::vector<Cluster>& oaClusters);
	
private:
	void SplitClusters();
	void Split(const Cluster& iCluster, std::vector<Cluster>& oaClusters);
	void FindBorders(std::vector<Border>& oaBorders);
	void RemoveInnerClusters(std::vector<Border>& ioaBorders);


	std::vector<Cluster> _aClusters;
	size_t _imgW, _imgH;
};

