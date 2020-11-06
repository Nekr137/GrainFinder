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
		res[i] = std::pow(res[i], 0.2);
	}
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



void Channel::ApplyMask(const Mask& iMask, Channel& oChannel) {

	std::cout << "\nApplying a mask...";

	const size_t maskSize = iMask.first.size();
	const size_t ctr = (iMask.first.size() - 1) / 2;

	oChannel = Channel(_w, _h);

	for (size_t i = 0; i < _w - maskSize; ++i) {
		for (size_t j = 0; j < _h - maskSize; ++j) {

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
}


Channel Channel::ApplyMask(const Mask& iMask) {
	Channel ch;
	ApplyMask(iMask, ch);
	return ch;
}