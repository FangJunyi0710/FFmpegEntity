#pragma once

#include "Formats.h"
#include <deque>
#include <memory>

extern "C"{
#include <libavformat/avformat.h>
}

class QImage;
namespace FFmpeg{

using std::deque;

class Frame{
	AVFrame* m_data=nullptr;
public:
	Frame();
	~Frame()noexcept;
	SWAP(Frame){
		std::swap(m_data,o.m_data);
	}
	COPY(Frame):m_data(av_frame_clone(o.m_data)){
		if(!m_data){
			m_data=av_frame_alloc();
		}
		if(!m_data){
			throw MemoryError("Failed to allocate AVFrame");
		}
	}
	CLONE(clone,Frame,Frame*)
    void unref()const;

    AVFrame* operator*(){return m_data;}
    const AVFrame* operator*()const{return m_data;}
    AVFrame* operator->(){return m_data;}
    const AVFrame* operator->()const{return m_data;}
};

class VideoFrame{
	struct Impl{
		int width = 0;
		int height = 0;
		Color* data=nullptr;
		Impl(){}
		Impl(int width_,int height_,const Color& color=Color()):width(std::max(0,width_)),height(std::max(0,height_)),data(width*height>0 ? reinterpret_cast<Color*>(new Color::DATA[width*height]) : nullptr){
			if(data){std::fill_n(reinterpret_cast<Color::DATA*>(data), width*height, color.data);}
		}
		void ref(){++refCount;}
		void unref(){--refCount;if(!refCount){delete this;}}
		bool writable(){return refCount==1;}
	private:
		uint refCount=1;
		~Impl(){delete[] data;}
	};
	Impl* impl;
	void copyImpl(){
		cTrace("copy()");
		Impl* newImpl=new Impl(impl->width,impl->height);
		memcpy(newImpl->data, impl->data, width() * height() * sizeof(Color));
		impl->unref();
		impl=newImpl;
	}
public:
	SWAP(VideoFrame){
		std::swap(impl,o.impl);
	}
	COPY(VideoFrame){
		impl=o.impl;
		o.impl->ref();
		cTrace("ref()");
	}
	CLONE(clone, VideoFrame, VideoFrame *)
	~VideoFrame()noexcept{impl->unref();}

	VideoFrame();
	VideoFrame(int width,int height,const Color& color=Color());
	VideoFrame(Frame frame);
	void clear();
	int width()const {return impl->width;}
	int height()const {return impl->height;}
	bool empty()const {return !width() || !height();}
	
	const Color &pixel(int w, int h) const { 
		if(w<0 || h<0 || w>=width() || h>=height()){
			throw FFmpegError("pixel() Index overflow");
		}
		return impl->data[h * width() + w]; 
	}
	void setPixel(int w, int h,const Color& newColor) { 
		if(w<0 || h<0 || w>=width() || h>=height()){
			throw FFmpegError("pixel() Index overflow");
		}
		if(!impl->writable()){
			copyImpl();
		} 
		impl->data[h * width() + w]=newColor; 
	}	
	void addToPixel(int w, int h,const Color& addition) { 
		if(w<0 || h<0 || w>=width() || h>=height()){
			throw FFmpegError("pixel() Index overflow");
		}
		if(!impl->writable()){
			copyImpl();
		} 
		impl->data[h * width() + w]+=addition; 
	}

	VideoFormat format() const;
	Frame toFrame()const;
	Frame toFrame(VideoFormat resFormat)const;

	VideoFrame(const QImage&);
	operator QImage()const;
};

class AudioBuffer{
	mutable uint sampleBytes_=0;
	AudioFormat m_format;
	vector<deque<uint8_t>> data;
	AudioFormat curFormat;
	SwResample converter;
	void flushConverter();
public:
	AudioBuffer(AudioFormat fmt);
	int size()const{return data.empty()?0:data[0].size()/sampleBytes()+converter.samplesCount();}
	AudioFormat format()const{return m_format;}
	int sampleBytes()const{return sampleBytes_?sampleBytes_:sampleBytes_=av_get_bytes_per_sample(format().sampleFormat);}
	
	void push(const vector<Frame>& frames);
	Frame pop(int frameSize);
	vector<Frame> pop(int frameSize,int count);
	vector<Frame> flush(int frameSize);
};

}