#pragma once

#include "Basic.h"
#include <iostream>

extern "C"{
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
typedef unsigned int QRgb;

namespace FFmpeg{

class Color{
	// Ensure consistent RGBA order regardless of byte order
	enum Shifts {
#if AV_HAVE_BIGENDIAN
		RedShift = 24,
		GreenShift = 16,
		BlueShift = 8,
		AlphaShift = 0
#else // little endian:
		RedShift = 0,
		GreenShift = 8,
		BlueShift = 16,
		AlphaShift = 24
#endif
	};
public:
	// 后者在上
	Color operator+(const Color& a) const {
		Color::DATA aalpha=a.alpha();
		if(aalpha==Color::max || alpha()==0){
			return a;
		}
		if(aalpha==0 || alpha()==Color::max){
			return *this;
		}
		Color::DATA balpha=alpha()*Color::T(Color::max-aalpha)/Color::max;
		return Color(avg(a.red(),red(),aalpha,balpha),avg(a.green(),green(),aalpha,balpha),avg(a.blue(),blue(),aalpha,balpha),aalpha+balpha);
	}

	Color operator*(double b) const {
		auto t = *this;
		return t *= b;
	}

	Color operator/(double b) const {
		auto t = *this;
		return t /= b;
	}
	using DATA=uint;
	using T = u_char;

	static const T max = 255;
	DATA data;
	Color():data(0){}
	Color(T r, T g, T b, T a = max) : data((DATA(r) << RedShift) | (DATA(g) << GreenShift) | (DATA(b) << BlueShift) | (DATA(a) << AlphaShift)) {}
	Color &operator+=(const Color &o) { return *this = *this + o; }
	Color &operator*=(double b) {setAlpha(lim(DATA(alpha())*b,0.0,max*1.0));return *this;}
	Color &operator/=(double b) {setAlpha(lim(DATA(alpha())/b,0.0,max*1.0));return *this;}
	T red() const { return (data >> RedShift) & max; }
	T green() const { return (data >> GreenShift) & max; }
	T blue() const { return (data >> BlueShift) & max; }
	T alpha() const { return (data >> AlphaShift) & max; }
	void setRed(T r) { data = (data & (~DATA() ^ (DATA(max) << RedShift))) | (DATA(r) << RedShift); }
	void setGreen(T g) { data = (data & (~DATA() ^ (DATA(max) << GreenShift))) | (DATA(g) << GreenShift); }
	void setBlue(T b) { data = (data & (~DATA() ^ (DATA(max) << BlueShift))) | (DATA(b) << BlueShift); }
	void setAlpha(T a) { data = (data & (~DATA() ^ (DATA(max) << AlphaShift))) | (DATA(a) << AlphaShift); }
	static AVPixelFormat PIX_FMT;

	explicit Color(const QRgb&);
	operator QRgb()const;
};

inline Color operator*(double b, const Color& a){
	auto t=a;
	return t*=b;
}

}

namespace std{
std::ostream& operator<<(std::ostream& out,const FFmpeg::Color& c);
}
