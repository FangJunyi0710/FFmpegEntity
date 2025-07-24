#include "Color.h"
#include <iostream>

std::ostream& std::operator<<(std::ostream& out,const myFFmpeg::Color& c){
	return out<<"RGBA("<<myFFmpeg::Color::RGBA(c.red())<<","<<myFFmpeg::Color::RGBA(c.green())<<","<<myFFmpeg::Color::RGBA(c.blue())<<","<<myFFmpeg::Color::RGBA(c.alpha())<<")";
}

namespace myFFmpeg{

#if AV_HAVE_BIGENDIAN
AVPixelFormat Color::PIX_FMT=AV_PIX_FMT_RGBA;
#else
AVPixelFormat Color::PIX_FMT=AV_PIX_FMT_ABGR;
#endif

Color operator+(const Color& a,const Color& b){
	Color::RGBA aalpha=a.alpha();
	Color::RGBA balpha=b.alpha()*Color::T(Color::max-aalpha)/Color::max;
	if(aalpha+balpha==0){
		return Color();
	}
	return Color(avg(a.red(),b.red(),aalpha,balpha),avg(a.green(),b.green(),aalpha,balpha),avg(a.blue(),b.blue(),aalpha,balpha),aalpha+balpha);
}

}
