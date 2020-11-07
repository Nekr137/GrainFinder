#pragma once

#include <vector>

class MathUtils
{
public:
  static std::vector<double> Diff(const std::vector<double>& iData, const size_t n);
private:
  static std::vector<double> Diff(const std::vector<double>& iData);
};

