#pragma once
#include <vector>

class Edge
{
public:
	Edge();
	~Edge();

private:
	std::vector<size_t> _x, _y; // coordinates
};

