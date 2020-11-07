#include "MathUtils.h"


std::vector<double> MathUtils::Diff(const std::vector<double>& iData, const size_t n) {

  std::vector<double> res = iData;
  for (size_t i = 0; i < n; ++i) {
    res = Diff(res);
  }
  return res;
}


std::vector<double> MathUtils::Diff(const std::vector<double>& iData) {
  const size_t n = iData.size();
  std::vector<double> res(n - 1);
  for (size_t i = 0; i < n-1; ++i) {
    res[i] = iData[i + 1] - iData[i];
  }
  return res;
}