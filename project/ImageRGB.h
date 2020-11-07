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

	enum Brightness { Human, Mean };
	Channel GetBrightness(const Brightness& iBrightnessType);

	void Save(const std::string& iFilename);
	void Show(const std::string& iTitle = "title");

private:
	Channel _r, _g, _b;
	size_t _w = 0, _h = 0;
};

