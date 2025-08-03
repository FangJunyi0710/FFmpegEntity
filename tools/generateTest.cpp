#include "AVInput.h"
#include "AVOutput.h"
#include "VideoEncoder.h"
#include "AudioEncoder.h"
#include <functional>
#include <iostream>
#include <memory>
using namespace std;
using namespace myFFmpeg;

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

int main(){
	AVOutput output("out/output.mp4", {new VideoEncoder({.id=AV_CODEC_ID_H264,.bitRate=30000,.options={{"preset","ultrafast"}}},{1920, 1080, AV_PIX_FMT_YUV420P})});
	
	vector<Layer> layer;
	layer.push_back(Layer({[](VideoFrame &frame, double t)
	{
		int x=(t-floor(t/1)*1)*Color::max;
		Color color1(x, 0, 0);
		cDebug(color1);
		Color color2(0, x, 0);
		Color color3(x, x, x);
		Color color4(0, 0, x);
		for(int i=0;i<1920;++i){
			for(int j=0;j<1080;++j){
				if(i<960 && j<540){
					frame.pixel(i,j)=color1;
				}
				if(i>=960 && j<540){
					frame.pixel(i,j)=color2;
				}
				if(i<960 && j>=540){
					frame.pixel(i,j)=color3;
				}
				if(i>=960 && j>=540){
					frame.pixel(i,j)=color4;
				}
			}
		}
	}}));

	const int len=3;
	for(int i=0;i<30*len;++i){
		if(i%30==0){
			clog<<i/30<<endl;
		}
		VideoFrame frame(1920,1080,Color());
		for(int j=0;j<(int)layer.size();++j){
			layer[j].produce(frame,i/30.0);
		}

		output[0]<<frame.toFrame();
	}
	output.flush();

	return 0;
}