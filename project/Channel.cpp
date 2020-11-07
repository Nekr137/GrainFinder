#include "Channel.h"
#include "Converter.h"
#include <assert.h>
#include <iostream>

Channel::Channel(size_t w, size_t h)
	: _w(w)
	, _h(h) {

	_data = std::vector<std::vector<double>>(w, std::vector<double>(h, 0.0));

}
Channel::Channel(const std::vector<std::vector<double>>& iData, size_t w, size_t h)
	: _data(iData)
	, _w(w)
	, _h(h) {
}
void Channel::Save(const std::string& iFilename) const {
	CImgWrapper wrapper;
	Converter::Convert(*this, wrapper);
	wrapper.Save(iFilename);
}
void Channel::Show(const std::string& iTitle) const {
	CImgWrapper wrapper;
	Converter::Convert(*this, wrapper);
	wrapper.Show(iTitle);
}
Channel& Channel::Print() {
	std::cout << "\nChannel:\n";
	for (size_t j = 0; j < _h; ++j) {
		for (size_t i = 0; i < _w; ++i) {
			std::cout << _data[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
  return *this;
}
Channel Channel::GradientRectangle(size_t iWidth, size_t iHeight) {

	// Changes to the right
	Channel rect(iWidth, iHeight);
	double step = 1.0 / iWidth;
	for (size_t i = 0; i < iWidth; ++i) {
		double color = static_cast<double>(i)* step;
		for (size_t j = 0; j < iHeight; ++j){
			rect.Set(i, j, color);
		}
	}
	return rect;
}
std::vector<double> Channel::Distribution() const {

	std::vector<double> res(256, 0.0);

	Channel copy = Copy();
	copy.Normalize();

	std::vector<unsigned int> distr(256);
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			double c = copy.Get(i, j);
			assert(c >= 0.0 - 1e-15 && c <= 1.0 + 1e-15);
			unsigned char color = static_cast<unsigned char>(c * 255.0);
			distr[color] += 1;
		}
	}
	unsigned int max = distr.front();
	for (auto v : distr) {
		if (max < v)
			max = v;
	}
	for (size_t i = 0; i < 256; ++i) {
		res[i] = static_cast<double>(distr[i]) / static_cast<double>(max);
		//res[i] = std::pow(res[i], 0.2);
	}

	for (auto& d : res) d = std::sqrt(d);
	return res;
}

Channel Channel::AsBitMap(const double& iThreshold) const {
	assert(iThreshold > 1.0 + 1e-12);
	assert(iThreshold < -1e-12);
	Channel bitmap = Copy();
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			double value = bitmap.Get(i, j);
			bitmap.Set(i, j, value > iThreshold ? 1.0 : 0.0);
		}
	}
	return bitmap;
}

double Channel::FindMean() const {
  double mean = 0.0;
  for (size_t i = 0; i < _w; ++i) {
    for (size_t j = 0; j < _h; ++j) {
      double value = _data[i][j];
      mean += value;
    }
  }
  return mean / _w / _h;
}

double Channel::FindMedian() const {
  std::vector<double> v;
  for (size_t i = 0; i < _w; ++i) {
    for (size_t j = 0; j < _h; ++j) {
      double value = _data[i][j];
      v.push_back(value);
    }
  }
  std::sort(v.begin(), v.end());
  return v[v.size()/2];
}

double Channel::FindSigma() const {
  double mean = FindMean();
  double mean2 = mean*mean;
  double sigma = 0.0;
  for (size_t i = 0; i < _w; ++i) {
    for (size_t j = 0; j < _h; ++j) {
      double value = _data[i][j];
      double d = value - mean;
      sigma += std::sqrt(d*d);
    }
  }
  double n = static_cast<double>(_w + _h);
  return sigma / std::sqrt(n) / n;
}

double Channel::FindMinElement() const {
	double min = _data.front().front();
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			double value = _data[i][j];
			if (value < min)
				min = value;
		}
	}
	return min;
}

double Channel::FindMaxElement() const {
	double max = _data.front().front();
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			double value = _data[i][j];
			if (value > max)
				max = value;
		}
	}
	return max;
}

void Channel::FindMinMaxElement(double& oMin, double& oMax) const {
	oMin = _data.front().front();
	oMax = _data.front().front();
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			double value = _data[i][j];
			if (value > oMax)
				oMax = value;
			if (value < oMin)
				oMin = value;
		}
	}
}

Channel Channel::Copy() const {
	Channel copy(_w, _h);
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			copy.Set(i, j, Get(i, j));
		}
	}
	return copy;
}

Channel Channel::GetInRange(const double iColorMin, const double iColorMax,
                            const double iColorForLower, const double iColorForUpper) const {

  auto inRange = [iColorMin, iColorMax](const double iValue) {return iColorMin <= iValue && iColorMax >= iValue; };

  Channel res(_w, _h);
  for (size_t i = 0; i < _w; ++i) {
    for (size_t j = 0; j < _h; ++j) {
      double val = _data[i][j];
      if (val < iColorMin)
        res.Set(i, j, iColorForLower);
      else if (val > iColorMax)
        res.Set(i, j, iColorForUpper);
      else
        res.Set(i, j, val);
    }
  }
  return res;
}

Channel& Channel::Normalize() {

	double min, max;
	FindMinMaxElement(min, max);

	double delta = max - min;
	double shift = -min;

	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			double& value = _data[i][j];
			value += shift;
			value /= delta;
		}
	}
	return *this;
}

Channel& Channel::Pow(const double iValue) {
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			auto d = _data[i][j];
			auto p = std::pow(d, iValue);
			_data[i][j] = p;
		}
	}
	return *this;
}

Channel& Channel::Abs() {
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			_data[i][j] = std::abs(_data[i][j]);
		}
	}
	return *this;
}

Channel Channel::operator +(const Channel& iOther) {

	assert(_w == iOther.GetWidth());
	assert(_h == iOther.GetHeight());

	Channel res(_w, _h);
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			double value = 0.5 * (_data[i][j] + iOther.Get(i, j));
			res.Set(i, j, value);
		}
	}
	return res;
}

Channel& Channel::Invert() {
	for (size_t i = 0; i < _w; ++i) {
		for (size_t j = 0; j < _h; ++j) {
			double value = _data[i][j];
			_data[i][j] = 1.0 - value;
		}
	}
	return *this;
}

Channel& Channel::RemoveOutlets(const double iSigmas) {

  double mean = FindMean();
  double sigma = FindSigma();
  //double median = FindMedian();
  double l = mean - iSigmas * sigma;
  double h = mean + iSigmas * sigma;
  (*this) = GetInRange(l, h, mean, mean);
  return *this;
}

Channel& Channel::InvertFrom(const double iColor) {
  for (size_t i = 0; i < _w; ++i) {
    for (size_t j = 0; j < _h; ++j) {
      double value = _data[i][j];
      if (value < iColor) {
        double dif = iColor - value;
        _data[i][j] = value + dif;
      }
    }
  }
  return *this;
}

void Channel::Crop(size_t left, size_t right, size_t top, size_t bottom, Channel& oCrop) {
	assert(_w > left + right);
	assert(_h > top + bottom);
	oCrop = Channel(_w - left - right, _h - top - bottom);
	for (size_t i = left; i < _w - right; ++i) {
		for (size_t j = top; j < _h - bottom; ++j) {
			double value = _data[i][j];
			oCrop.Set(i-left, j-top, value);
		}
	}
}

void Channel::ApplyMask(const Mask& iMask, Channel& oChannel) {

	std::cout << "\nApplying a mask...";

	const size_t maskSize = iMask.first.size();
	const size_t ctr = (iMask.first.size() - 1) / 2;

	oChannel = Channel(_w, _h);

	for (size_t i = 0; i < _w - maskSize + 1; ++i) {
		for (size_t j = 0; j < _h - maskSize + 1; ++j) {

			double value = 0.0;

			for (size_t mi = 0; mi < maskSize; ++mi) {
				for (size_t mj = 0; mj < maskSize; ++mj) {
					size_t x = i + mi;
					size_t y = j + mj;
					double coef = iMask.first[mi][mj];
					value += coef * Get(x, y);
				}
			}

			value /= iMask.second;
			oChannel.Set(i + ctr, j + ctr, value);
		}
	}

  // borders
  for (size_t k = 0; k < ctr; k++) {
    size_t d = ctr - k;

    // corners
    oChannel.Set(d-1, d-1, oChannel.Get(d, d));
    oChannel.Set(_w-d, d-1, oChannel.Get(_w-d-1, d));
    oChannel.Set(d-1, _h-d, oChannel.Get(d, _h-d-1));
    oChannel.Set(_w-d, _h-d, oChannel.Get(_w-d-1, _h-d-1));

    // lines
    for (size_t side = 0; side < 2; ++side) {
      for (size_t i = d-1; i < _w-d; ++i) {
        oChannel.Set(i,d-1, oChannel.Get(i, d));
        oChannel.Set(i,_h-d, oChannel.Get(i,_h-d-1));
      }
      for (size_t i = d; i < _h - d; ++i) {
        oChannel.Set(d-1, i, oChannel.Get(d, i));
        oChannel.Set(_w-d,i, oChannel.Get(_w-d-1,i));
      }
    }
  }
}


Channel Channel::ApplyMask(const Mask& iMask) {
	Channel ch;
	ApplyMask(iMask, ch);
	return ch;
}