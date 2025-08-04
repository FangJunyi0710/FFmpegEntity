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
	int m_width = 0;
	int m_height = 0;
	Color* m_data;
public:
	SWAP(VideoFrame){
		std::swap(m_width,o.m_width);
		std::swap(m_height,o.m_height);
		std::swap(m_data,o.m_data);
	}
	COPY(VideoFrame) : VideoFrame(o.m_width,o.m_height){
		memcpy(m_data, o.m_data, m_width * m_height * sizeof(Color));
	}
	CLONE(clone, VideoFrame, VideoFrame *)
	~VideoFrame()noexcept{delete[] m_data;}

	VideoFrame();
	VideoFrame(int width,int height,const Color& color=Color());
	VideoFrame(Frame frame);
	void clear();
	int width()const {return m_width;}
	int height()const {return m_height;}
	bool empty()const {return !m_width || !m_height;}
	void setWidth(int w);
	void setHeight(int h);
	
	Color &pixel(int w, int h) { return m_data[h * m_width + w]; }
	const Color &pixel(int w, int h) const { return m_data[h * m_width + w]; }
	Color* data(){return m_data;}
	const Color* data()const{return m_data;}

	VideoFormat format() const;
	Frame toFrame()const;
	Frame toFrame(VideoFormat resFormat)const;

	VideoFrame(const QImage&);
	operator QImage()const;
};

class AudioBuffer{
	AudioFormat m_format;
	vector<deque<vector<uint8_t>>> data;
	AudioFormat curFormat;
	SwResample converter;
	void flushConverter();
public:
	AudioBuffer(AudioFormat fmt);
	void push(const vector<Frame>& frames);
	int size()const;
	AudioFormat format()const;
	int sampleBytes()const;
	Frame pop(int frameSize);
	vector<Frame> pop(int frameSize,int count);
	vector<Frame> flush(int frameSize);
};

}