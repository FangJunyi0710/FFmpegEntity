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
public:
	using RGBA=uint;
	using T = u_char;

	static const u_char max = 0xff;
	uint rgba = 0x000000ff;
	Color(uint rgba_ = 0x000000ff) : rgba(rgba_) {}
	Color(u_char r, u_char g, u_char b, u_char a = max) : rgba((uint(r) << 24) | (uint(g) << 16) | (uint(b) << 8) | a) {}
	Color &operator+=(const Color &o) { return *this = *this + o; }
	u_char red() const { return (rgba >> 24) & 0xff; }
	u_char green() const { return (rgba >> 16) & 0xff; }
	u_char blue() const { return (rgba >> 8) & 0xff; }
	u_char alpha() const { return rgba & 0xff; }
	void setRed(u_char r) { rgba = (rgba & 0x00ffffff) | (uint(r) << 24); }
	void setGreen(u_char g) { rgba = (rgba & 0xff00ffff) | (uint(g) << 16); }
	void setBlue(u_char b) { rgba = (rgba & 0xffff00ff) | (uint(b) << 8); }
	void setAlpha(u_char a) { rgba = (rgba & 0xffffff00) | a; }
	static AVPixelFormat PIX_FMT;
};

template<class W>
Color avg(Color a,Color b,W wa){
	return Color(avg(a.rgba,b.rgba,wa));
}

}

namespace std{
std::ostream& operator<<(std::ostream& out,const myFFmpeg::Color& c);
}
