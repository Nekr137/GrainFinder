#pragma once

#include <vector>

typedef std::pair<std::vector<std::vector<double>>, double> Mask;

class Channel
{
public:
	Channel() {}
	Channel(size_t w, size_t h);
	Channel(const std::vector<std::vector<double>>& iData, size_t w, size_t h);

	size_t GetWidth() const { return _w; }
	size_t GetHeight() const { return _h; }

  double FindMean() const;
  double FindMedian() const;
  double FindSigma() const;
	double FindMinElement() const;
	double FindMaxElement() const;	
	void FindMinMaxElement(double& oMin, double& oMax) const;

	void Set(size_t iCol, size_t iRow, double iValue) { _data[iCol][iRow] = iValue; }
	double Get(size_t iCol, size_t iRow) const { return _data[iCol][iRow]; }

	Channel Copy() const;
  Channel GetInRange(const double iColorMin, const double iColorMax,
                     const double iColorForLower, const double iColorForUpper) const;

	void ApplyMask(const Mask& iMask, Channel& oChannel);
	Channel ApplyMask(const Mask& iMask);

	Channel& Normalize();
	Channel& Abs();
	Channel& Pow(const double iValue);
	Channel& Invert();
  Channel& RemoveOutlets(const double iSigmas);
  Channel& InvertFrom(const double iColor);
  Channel& Print();
	
	void Crop(size_t left, size_t right, size_t top, size_t bottom, Channel& oCrop);

	void Save(const std::string& iFilename) const;
	void Show(const std::string& iTitle = "title") const;

	static Channel GradientRectangle(size_t iWidth, size_t iHeight);

	std::vector<double> Distribution() const;
	Channel AsBitMap(const double& iThreshold) const;

	Channel operator +(const Channel& iOther);

private:
	// pixel color from 0.0 to 1.0
	std::vector<std::vector<double>> _data; // [col][row]
	size_t _w = 0, _h = 0;
};

