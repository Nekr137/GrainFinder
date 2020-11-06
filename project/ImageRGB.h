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

	Channel GetBrightness();

	void Save(const std::string& iFilename);

private:
	Channel _r, _g, _b;
	size_t _w = 0, _h = 0;
};

