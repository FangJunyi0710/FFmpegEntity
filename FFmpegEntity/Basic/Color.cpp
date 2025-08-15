#include "Color.h"
#include <iostream>

std::ostream& std::operator<<(std::ostream& out,const FFmpeg::Color& c){
	return out<<"RGBA("<<FFmpeg::Color::DATA(c.red())<<","<<FFmpeg::Color::DATA(c.green())<<","<<FFmpeg::Color::DATA(c.blue())<<","<<FFmpeg::Color::DATA(c.alpha())<<")";
}

namespace FFmpeg{

AVPixelFormat Color::PIX_FMT=AV_PIX_FMT_RGBA;

}
