#include <QImage>
#include <QDir>
#include <QDirIterator>
#include <algorithm>
#include <iostream>
QVector<QImage> getImages(){
	QVector<QImage> ret;
	QStringList filters;
	filters << "*.png";
	QStringList files;
	QDirIterator it("resource", filters, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext()){
		files.append(it.next());
	}
	std::sort(files.begin(), files.end());
	for (const QString &file : files){
		ret.append(QImage(file));
	}
	return ret;
}
int main(){
	QVector<QImage> imgs=getImages();
	int width=0,height=0;
	for (QImage& img : imgs){
		width+=img.width();
		height=std::max(height,img.height());
	}
	qDebug()<<width<<"x"<<height;
	QImage output(width,height,QImage::Format_ARGB32);
	int curx=0;
	for (QImage& img : imgs){
		for(int i=0;i<img.width();++i){
			for(int j=0;j<img.height();++j){
				output.setPixel(i+curx,j,img.pixel(i,j));
			}
		}
		curx+=img.width();
		std::clog<<curx<<"/"<<width<<"\r";
	}
	output.save("out/stick.png");
	return 0;
}