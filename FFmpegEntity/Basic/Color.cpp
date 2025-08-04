#include "Color.h"
#include <iostream>

std::ostream& std::operator<<(std::ostream& out,const FFmpeg::Color& c){
	return out<<"RGBA("<<FFmpeg::Color::DATA(c.red())<<","<<FFmpeg::Color::DATA(c.green())<<","<<FFmpeg::Color::DATA(c.blue())<<","<<FFmpeg::Color::DATA(c.alpha())<<")";
}

namespace FFmpeg{

AVPixelFormat Color::PIX_FMT=AV_PIX_FMT_RGBA;

// a 在上，b 在下
Color operator+(const Color& a,const Color& b){
	Color::DATA aalpha=a.alpha();
	if(aalpha==Color::max || b.alpha()==0){
		return a;
	}
	if(aalpha==0 || b.alpha()==Color::max){
		return b;
	}
	Color::DATA balpha=b.alpha()*Color::T(Color::max-aalpha)/Color::max;
	return Color(avg(a.red(),b.red(),aalpha,balpha),avg(a.green(),b.green(),aalpha,balpha),avg(a.blue(),b.blue(),aalpha,balpha),aalpha+balpha);
}

}
