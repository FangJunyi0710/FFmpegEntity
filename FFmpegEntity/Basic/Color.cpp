#include "Color.h"
#include <iostream>

std::ostream& std::operator<<(std::ostream& out,const myFFmpeg::Color& c){
	return out<<"RGBA("<<myFFmpeg::Color::RGBA(c.red())<<","<<myFFmpeg::Color::RGBA(c.green())<<","<<myFFmpeg::Color::RGBA(c.blue())<<","<<myFFmpeg::Color::RGBA(c.alpha())<<")";
}

namespace myFFmpeg{

AVPixelFormat Color::PIX_FMT=AV_PIX_FMT_RGBA64;

// a 在上，b 在下
Color operator+(const Color& a,const Color& b){
	Color::RGBA aalpha=a.alpha();
	if(aalpha==Color::max || b.alpha()==0){
		return a;
	}
	if(aalpha==0 || b.alpha()==Color::max){
		return b;
	}
	Color::RGBA balpha=b.alpha()*Color::T(Color::max-aalpha)/Color::max;
	return Color(avg(a.red(),b.red(),aalpha,balpha),avg(a.green(),b.green(),aalpha,balpha),avg(a.blue(),b.blue(),aalpha,balpha),aalpha+balpha);
}

}
