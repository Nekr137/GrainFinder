#pragma once

#include "Channel.h"


class ImageRGB
{
public:
	ImageRGB() {}
	ImageRGB(const Channel& iR, const Channel& iG, const Channel& iB);

	size_t GetWidth() const { return _w; }
	size_t GetHeight() const { return _h; }

	const Channel& GetRedChannelRef() const { return _r; }
	const Channel& GetGreenChannelRef() const { return _g; }
	const Channel& GetBlueChannelRef() const { return _b; }

	Channel& GetRedChannelRef() { return _r; }
	Channel& GetGreenChannelRef() { return _g; }
	Channel& GetBlueChannelRef() { return _b; }

	void Get(const size_t i, const size_t j, double& oR, double& oG, double& oB) const;
	void Set(const size_t i, const size_t j, const double iR, const double iG, const double iB);

	enum Brightness { Human, Mean };

	static double GetBrightness(const double r, const double g, const double b, const Brightness& iBrightnessType);
	Channel GetBrightness(const Brightness& iBrightnessType);

	void Save(const std::string& iFilename);
	void Show(const std::string& iTitle = "title", const int milliseconds = -1);

	bool AskUserAboutAColor(double& oR, double& oG, double& oB);

	void DrawLine(int iX0, int iY0, int iX1, int iY1, std::vector<int>& oX, std::vector<int>& oY);

private:
	Channel _r, _g, _b;
	size_t _w = 0, _h = 0;

	struct BrightnessCoeff
	{
		static std::vector<double> Human;
		static std::vector<double> Mean;
	};
};

