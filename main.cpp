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
	virtual double mapTick(double tick){
		return tick;
	}
public:
	void produce(VideoFrame& frame,double tick){
		generate(frame,mapTick(tick));
	}
	virtual ~AbstractLayer()=default;
};
class Layer:public AbstractLayer{
	function<void(VideoFrame&,double)> gen;
	function<double(double)> map;
public:
	void generate(VideoFrame& frame,double tick)override{
		return gen(frame,tick);
	}
	double mapTick(double tick)override{
		return map(tick);
	}
	Layer(function<void(VideoFrame&,double)> gen_,function<double(double)> map_=[](double x){return x;}):gen(gen_),map(map_){}
	void bindGenerator(function<void(VideoFrame&,double)> newGen){
		gen=newGen;
	}
	void bindMapper(function<double(double)> newMap){
		map=newMap;
	}
};

int main(){
	AVOutput output("out/output.mp4", {new VideoEncoder({1920, 1080, AV_PIX_FMT_YUV420P},AV_CODEC_ID_H264,30,300000,{{"preset","ultrafast"}})}); // 创建输出对象，指定文件名、分辨率和帧率
	
	AVInput mp3("resource/起风了.mpga");
	auto& stream=mp3.stream(AVMEDIA_TYPE_AUDIO);
	stream.moveTo(30);
	AVOutput tmpOutput("out/tmp.mp3",{new AudioEncoder({AV_CHANNEL_LAYOUT_STEREO,AV_SAMPLE_FMT_FLTP,44100},AV_CODEC_ID_MP3)});
	tmpOutput.stream(AVMEDIA_TYPE_AUDIO).encode(stream.decode(10));
	tmpOutput.close();
	
	vector<unique_ptr<AbstractLayer>> layer;
	layer.push_back(make_unique<Layer>([](VideoFrame &frame, double t)
	{
		int x=(t-floor(t/1)*1)*Color::max;
		Color color1(x, 0, 0, x);
		Color color2(0, x, 0, x);
		Color color3(x, x, x, x);
		Color color4(0, 0, x, x);
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
	},
	[](double x)
	{
		return x;
	}));
	const int len=10;
	for(int i=0;i<30*len;++i){
		if(i%30==0){
			clog<<i/30<<endl;
		}
		VideoFrame frame(1920,1080,Color());
		for(int j=0;j<(int)layer.size();++j){
			layer[j]->produce(frame,i/30.0);
		}

		output.stream(AVMEDIA_TYPE_VIDEO)<<frame.toFrame();
	}
	output.flush();

	return 0;
}