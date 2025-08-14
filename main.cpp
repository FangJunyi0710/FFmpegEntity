#include "AVInput.h"
#include "AVOutput.h"
#include "VideoEncoder.h"
#include "AudioEncoder.h"
#include <functional>
#include <iostream>
#include <memory>
#include <QImage>
#include <QPainter>
#include <QFont>
#include <iostream>
#include <vector>
#include <iomanip>
#include <QGuiApplication>
#include <QDate>
#include <QFontDatabase>
using namespace std;
using namespace FFmpeg;

class AbstractLayer{
protected:
	virtual void generate(VideoFrame& frame,double tick)=0;
public:
	void produce(VideoFrame& frame,double tick){
		generate(frame,tick);
	}
	virtual ~AbstractLayer()=default;
};
class Layer:public AbstractLayer{
	deque<function<void(VideoFrame&,double&)>> gen;
protected:
	void generate(VideoFrame& frame,double tick)override{
		for(auto& each:gen){
			each(frame,tick);
		}
	}
public:
	Layer()=default;
	Layer(const deque<function<void(VideoFrame&,double&)>>& funcs):gen(funcs){}
	void prepend(const function<void(VideoFrame&,double&)>& func){
		gen.push_front(func);
	}
	void append(const function<void(VideoFrame&,double&)>& func){
		gen.push_back(func);
	}
};

QSize calculateImageSize(const QString& text, const QFont& font) {
	QFontMetrics metrics(font);
	QStringList lines = text.split('\n'); // 按换行符分割多行文字
	int maxWidth = 0;
	int totalHeight = 0;

	// 计算最大行宽和总高度
	for (const QString& line : lines) {
		int lineWidth = metrics.horizontalAdvance(line);
		if (lineWidth > maxWidth) {
			maxWidth = lineWidth;
		}
		totalHeight += metrics.height();
	}
	return QSize(maxWidth, totalHeight);
}

// 将文字转换为像素矩阵
QImage textToQImage(const QString& text, const QFont& font, const QColor& color) {
	// 创建一个QImage对象，用于绘制文字
	QImage image(calculateImageSize(text, font),QImage::Format_ARGB32);
	image.fill(Qt::transparent);

	// 使用QPainter在QImage上绘制文字
	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setFont(font);
	painter.setPen(color); // 文字颜色为黑色
	painter.drawText(image.rect(), Qt::AlignLeft | Qt::AlignTop, text);
	painter.end();
	return image;
}

double time2tick(double time){
	return (time-4.050)*1000/1558.5194/2;
}

QDate tick2date(double tick){
	return QDate(2022,11,27).addDays(tick*7);
}

const double len=312.947;
const double tBegin=time2tick(0),tEnd=time2tick(len);
const double tLen=tEnd-tBegin;

string formatTick(double tick){
	int offset=0;
	while(tick<0){
		const int step=1000;
		tick+=step;
		offset+=step;
	}

    int total = tick * 16000;
	int beat=total/16000-offset;
    int step1 = total%16000/4000;
	int step2 = total%4000/1000;
    int extra = total%1000;
    
    std::ostringstream oss;
    oss << beat << ":" << step1 << ":" << step2 << ":" << std::setw(3) << std::setfill('0')<< extra;
    return oss.str();
}

// 进度条
Layer timeline(){
	Layer layer;
	layer.append([=](VideoFrame& f,double& t){
		const int barHeight=20;// 进度条高度
		const int TextSize=20;
		const int w=f.width(),h=f.height();
		const int hpos=h-barHeight;
		for(int i=0;i<(t-tBegin)/tLen*w;++i){
			for(int j=hpos;j<h;++j){
				f.pixel(i,j)+=Color(200,200,200);
			}
		}
		for(int i=0;i<w;++i){
			f.pixel(i,hpos)+=Color(255,255,255);
		}
		auto text=textToQImage(QString::fromStdString(formatTick(t)),QFont("Ubuntu Mono",TextSize),Qt::white);// 
		for(int i=0;i<text.width();++i){
			for(int j=0;j<text.height();++j){
				f.pixel(f.width()-text.width()+i,f.height()-text.height()-barHeight+j)+=text.pixel(i,j);
			}
		}
	});
	return layer;
}

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

	AVOutput output("out/output.mp4", {
		new VideoEncoder({.id=AV_CODEC_ID_H264,.options={{"preset","ultrafast"}}},{1920, 1080, AV_PIX_FMT_YUV420P},30),
		new AudioEncoder({.id=AV_CODEC_ID_AAC},{AV_CHANNEL_LAYOUT_STEREO,AV_SAMPLE_FMT_FLTP,44100})
	});
	AVInput bgm("resource/起风了.mpga");
	output[1]<<bgm[bgm.index(AVMEDIA_TYPE_AUDIO)].decode();
	
	vector<Layer> layer{
		timeline()
	};

	for(int i=0;i<30*len;++i){
		if(i%30==0){
			cInfo(i/30);
		}
		double tick=time2tick(i/30.0);

		VideoFrame frame(1920,1080,Color());
		for(int j=0;j<(int)layer.size();++j){
			layer[j].produce(frame,tick);
		}

		output[0]<<frame.toFrame();
	}
	output.flush();

	return 0;
}

#include "QtSupport.cpp"