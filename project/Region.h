#pragma once

#include "P2D.h"
#include <vector>

class Region
{
public:
	Region() {}
	
	void AddPnt(const P2D& iPnt) { _aPoints.push_back(iPnt); }

private:
	P2DVect _aPoints;
};

