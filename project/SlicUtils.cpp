#include "SlicUtils.h"


void FindLine(int iX0, int iY0, int iX1, int iY1, std::vector<int>& oX, std::vector<int>& oY) {

	/*
	https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	iX0, iY0 - start poin, iX1, iY1 - end point;
	*/

	int dx = std::abs(iX1 - iX0);
	int dy = std::abs(iY1 - iY0);

	bool reversed = dy > dx;

	if (reversed) {
		std::swap(iX0, iY0);
		std::swap(iX1, iY1);
		std::swap(dx, dy);
	}

	if (iX0 > iX1) {
		std::swap(iX0, iX1);
		std::swap(iY0, iY1);
	}

	double er = 0.0;
	double der = (dy + 1.0) / (dx + 1.0);
	int y = iY0;
	int diry = iY1 - iY0 > 0 ? 1 : -1;

	for (int x = iX0; x <= iX1; ++x) {
		oX.push_back(x);
		oY.push_back(y);
		er += der;
		if (er >= 1.0) {
			y += diry;
			er -= 1.0;
		}
	}

	if (reversed)
		std::swap(oX, oY);
}
