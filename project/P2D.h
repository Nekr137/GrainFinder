#pragma once
#include <vector>
class P2D;

typedef std::vector<P2D> P2DVect;

class P2D
{
public:
	P2D(){};
	P2D(size_t iX, size_t iY) : x(iX), y(iY) {}
	size_t X() const { return x; }
	size_t Y() const { return y; }

	size_t x = 0, y = 0;
};

