#pragma once

#include "Basic.h"
#include <iostream>

extern "C"{
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

namespace myFFmpeg{

class Color;
Color operator+(const Color& a,const Color& b);

class Color{
	// Ensure consistent RGBA order regardless of byte order
	enum Shifts {
#if AV_HAVE_BIGENDIAN
		RedShift = 48,
		GreenShift = 32,
		BlueShift = 16,
		AlphaShift = 0
#else // little endian:
		RedShift = 0,
		GreenShift = 16,
		BlueShift = 32,
		AlphaShift = 48
#endif
	};
public:
	using RGBA=ull;
	using T = ushort;

	static const T max = 0xffff;
	RGBA rgba;
	Color(RGBA rgba_ = 0x000000ff) : rgba(rgba_) {}
	Color(T r, T g, T b, T a = max) : rgba((RGBA(r) << RedShift) | (RGBA(g) << GreenShift) | (RGBA(b) << BlueShift) | (RGBA(a) << AlphaShift)) {}
	Color &operator+=(const Color &o) { return *this = *this + o; }
	T red() const { return (rgba >> RedShift) & max; }
	T green() const { return (rgba >> GreenShift) & max; }
	T blue() const { return (rgba >> BlueShift) & max; }
	T alpha() const { return (rgba >> AlphaShift) & max; }
	void setRed(T r) { rgba = (rgba & (~RGBA() ^ (RGBA(max) << RedShift))) | (RGBA(r) << RedShift); }
	void setGreen(T g) { rgba = (rgba & (~RGBA() ^ (RGBA(max) << GreenShift))) | (RGBA(g) << GreenShift); }
	void setBlue(T b) { rgba = (rgba & (~RGBA() ^ (RGBA(max) << BlueShift))) | (RGBA(b) << BlueShift); }
	void setAlpha(T a) { rgba = (rgba & (~RGBA() ^ (RGBA(max) << AlphaShift))) | (RGBA(a) << AlphaShift); }
	static AVPixelFormat PIX_FMT;
};

}

namespace std{
std::ostream& operator<<(std::ostream& out,const myFFmpeg::Color& c);
}
