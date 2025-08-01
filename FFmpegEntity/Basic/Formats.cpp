#include "Formats.h"
#include "Frame.h"
#include <map>
#include <mutex>

namespace myFFmpeg{

bool operator<(const VideoFormat& a,const VideoFormat& b){
	if(a.pix_fmt!=b.pix_fmt){
		return a.pix_fmt<b.pix_fmt;
	}
	if(a.width!=b.width){
		return a.width<b.width;
	}
	return a.height<b.height;
}

class SwscaleBuffer{
	std::map<std::pair<VideoFormat,VideoFormat>,SwsContext*> bufferedSwscale;
	std::mutex bufferMutex;
	void clear(){
		for(const auto& [f,p]:bufferedSwscale){
			sws_freeContext(p);
		}
		bufferedSwscale.clear();
	}
public:
	~SwscaleBuffer()noexcept{
		clear();
	}
	SwsContext* pop(const VideoFormat& src,const VideoFormat& dst){
		std::unique_lock<std::mutex> locker(bufferMutex);
		auto p=bufferedSwscale.find({src,dst});
		if(p==bufferedSwscale.end()){
			return nullptr;
		}
		auto ret=p->second;
		bufferedSwscale.erase(p);
		return ret;
	}
	void push(SwsContext* p,const VideoFormat& src,const VideoFormat& dst){
		if(!p || src.pix_fmt==-1 || dst.pix_fmt==-1){
			return;
		}
		std::unique_lock<std::mutex> locker(bufferMutex);
		if(bufferedSwscale.size()>=1024){
			clear();
		}
		bufferedSwscale[{src,dst}]=p;
	}
}bufferS;

Swscale::Swscale(const VideoFormat& src,const VideoFormat& dst):m_src(src),m_dst(dst){
	if(!(context=bufferS.pop(src,dst))){
		context=sws_getContext(src.width,src.height,src.pix_fmt,dst.width,dst.height,dst.pix_fmt,SWS_FAST_BILINEAR,nullptr,nullptr,nullptr);
		if(!context) {
			throw MemoryError("Failed to allocate SwsContext");
		}
	}
}

Frame Swscale::scale(const Frame& src)const{
	Frame ret;
    if(sws_scale_frame(context,*ret,*src) < 0) {
        throw ConvertError("Failed to scale frame");
    }
	return ret;
}
Swscale::~Swscale()noexcept{
	bufferS.push(context,m_src,m_dst);
}

VideoFormat::VideoFormat(const Frame& frame):VideoFormat(frame->width,frame->height,AVPixelFormat(frame->format)){}
bool operator==(const VideoFormat& a,const VideoFormat& b){
	return a.width==b.width && a.height==b.height && a.pix_fmt==b.pix_fmt;
}
bool operator!=(const VideoFormat& a,const VideoFormat& b){
	return !(a==b);
}

bool operator==(const AudioFormat& a,const AudioFormat& b){
	return !av_channel_layout_compare(&a.channelLayout,&b.channelLayout) && a.sampleFormat==b.sampleFormat && a.sampleRate==b.sampleRate;
}
bool operator!=(const AudioFormat& a,const AudioFormat& b){
	return !(a==b);
}

AudioFormat::AudioFormat(const Frame& frame):AudioFormat(frame->ch_layout,AVSampleFormat(frame->format),frame->sample_rate){}
AudioFormat::AudioFormat(AVChannelLayout ch_layout,AVSampleFormat sampleFormat,int sampleRate_):
	channelLayout(ch_layout),sampleFormat(sampleFormat),sampleRate(sampleRate_){}

SwResample::SwResample(AudioFormat src,AudioFormat dst){
	if(swr_alloc_set_opts2(&context,&dst.channelLayout,dst.sampleFormat,dst.sampleRate,&src.channelLayout,src.sampleFormat,src.sampleRate,0,nullptr) < 0 || !context) {
		throw MemoryError("Failed to allocate SwrContext");
	}
	if(swr_init(context) < 0) {
		swr_free(&context);
		throw ConvertError("Failed to initialize SwrContext");
	}
}
void SwResample::send(const uint8_t*const* in,int count)const{
	swr_convert(context,nullptr,0,const_cast<const uint8_t**>(in),count);
}
int SwResample::receive(uint8_t*const* out,int count)const{
	return swr_convert(context,const_cast<uint8_t**>(out),count,nullptr,0);
}
int SwResample::samplesCount()const{
	return swr_get_out_samples(context,0);
}
SwResample::~SwResample()noexcept{
	swr_free(&context);
}

}