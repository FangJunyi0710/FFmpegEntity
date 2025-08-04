#include "Frame.h"
#include <QImage>
namespace FFmpeg{


Color::Color(const QRgb& c):Color(qRed(c),qGreen(c),qBlue(c),qAlpha(c)){}
Color::operator QRgb()const{
	return qRgba(red(),green(),blue(),alpha());
}

VideoFrame::VideoFrame(const QImage& img):VideoFrame(img.width(),img.height()){
	for(int i=0;i<width();++i){
		for(int j=0;j<height();++j){
			pixel(i,j)=img.pixel(i,j);
		}
	}
}
VideoFrame::operator QImage()const{
	QImage ret(width(),height(),QImage::Format_ARGB32);
	for(int i=0;i<width();++i){
		for(int j=0;j<height();++j){
			ret.setPixel(i,j,pixel(i,j));
		}
	}
	return ret;
}


}