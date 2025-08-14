#include "Frame.h"
#include "Basic.h"
#include <cassert>
#include <memory>
#include <iostream>

namespace FFmpeg{

using std::min;

Frame::Frame():m_data(av_frame_alloc()){
	if(!m_data){
		throw MemoryError("Failed to allocate frame");
	}
}
Frame::~Frame()noexcept{
	if(m_data) {
		av_frame_free(&m_data);
	}
}
void Frame::unref()const{
	av_frame_unref(m_data);
}

VideoFrame::VideoFrame():m_width(0),m_height(0),m_data(nullptr){}
VideoFrame::VideoFrame(int width,int height,const Color& color):
	m_width(std::max(0,width)),
	m_height(std::max(0,height)),
	m_data(m_width*m_height>0 ? reinterpret_cast<Color*>(new Color::DATA[m_width*m_height]) : nullptr)
{
	if(m_data){
		// 直接填充内存，避免多次调用Color构造函数
		std::fill_n(reinterpret_cast<Color::DATA*>(m_data), m_width*m_height, color.data);
	}
}

VideoFrame::VideoFrame(FFmpeg::Frame frame):
	m_width(frame->width),
	m_height(frame->height),
	m_data(m_width*m_height>0 ? new Color[m_width*m_height] : nullptr)
{
	if(!m_data) return;
	
	frame=FFmpeg::Swscale(frame,{m_width,m_height,Color::PIX_FMT}).scale(frame);
	
	for(int i=0;i<m_height;++i){
		for(int j=0;j<m_width;++j){
			const Color::T* pixel=reinterpret_cast<const Color::T*>(
				frame->data[0]+frame->linesize[0]*i+4*sizeof(Color::T)*j);
			m_data[i*m_width+j]=Color(pixel[0],pixel[1],pixel[2],pixel[3]);
		}
	}
}

void VideoFrame::clear(){
	m_width = 0;
	m_height = 0;
	delete[] m_data;
	m_data=nullptr;
}
void VideoFrame::setWidth(int w){
	w = max(0, w);
	if(w == m_width) return;
	
	auto newData = w*m_height > 0 ? new Color[w*m_height] : nullptr;
	if(newData && m_data){
		for(int i=0; i<min(m_height, m_height); ++i){
			for(int j=0; j<min(m_width, w); ++j){
				newData[i*w + j] = m_data[i*m_width + j];
			}
		}
	}
	delete[] m_data;
	m_data=newData;
	m_width = w;
}

void VideoFrame::setHeight(int h){
	h = max(0, h);
	if(h == m_height) return;
	
	auto newData = m_width*h > 0 ? new Color[m_width*h] : nullptr;
	if(newData && m_data){
		memcpy(newData, m_data, m_width*min(m_height, h)*sizeof(Color));
	}
	delete[] m_data;
	m_data=newData;
	m_height = h;
}

FFmpeg::VideoFormat VideoFrame::format()const{
	return {m_width, m_height, Color::PIX_FMT};
}

FFmpeg::Frame VideoFrame::toFrame()const{
	FFmpeg::Frame ret;
	ret->width = m_width;
	ret->height = m_height;
	ret->format = Color::PIX_FMT;
	if(av_frame_get_buffer(*ret, 0) < 0){
		throw MemoryError("Failed to allocate frame buffer");
	}
	
	if(m_data){
		for(int i=0; i<m_height; ++i){
			memcpy(ret->data[0] + ret->linesize[0]*i, 
				  &m_data[i*m_width], 
				  min(static_cast<size_t>(ret->linesize[0]), m_width*sizeof(Color)));
		}
	}
	return ret;
}
FFmpeg::Frame VideoFrame::toFrame(FFmpeg::VideoFormat resFormat)const{
	return FFmpeg::Swscale(format(),resFormat).scale(toFrame());
}

void AudioBuffer::flushConverter(){
    std::vector<std::unique_ptr<uint8_t[]>> buffers;
    std::unique_ptr<uint8_t*[]> buf(new uint8_t*[m_format.channelLayout.nb_channels]);
    
    int len=converter.samplesCount();
    for(int i=0;i<m_format.channelLayout.nb_channels;++i){
        buffers.emplace_back(new uint8_t[len*sampleBytes()]);
        buf[i] = buffers.back().get();
    }
    
    converter.receive(buf.get(), len);
    
    for(int i=0;i<m_format.channelLayout.nb_channels;++i){
        for(int j=0;j<len;++j){
            data[i].push_back(vector<uint8_t>(buf[i]+j*sampleBytes(), buf[i]+(j+1)*sampleBytes()));
        }
    }
}
AudioBuffer::AudioBuffer(FFmpeg::AudioFormat fmt):
	m_format(fmt),data(m_format.channelLayout.nb_channels),curFormat(m_format),converter(curFormat,m_format){}
void AudioBuffer::push(const vector<FFmpeg::Frame>& frames){
	for(const auto& frame:frames){
		if(AudioFormat(frame).sampleFormat==-1){
			continue;
		}
		if(frame!=curFormat){
			flushConverter();
			converter=FFmpeg::SwResample(frame,m_format);
		}
        converter.send(frame->data,frame->nb_samples);
	}
}
int AudioBuffer::size()const{
	if(data.empty()){
		return 0;
	}
	return data[0].size()+converter.samplesCount();
}
FFmpeg::AudioFormat AudioBuffer::format()const{
	return m_format;
}
int AudioBuffer::sampleBytes()const{
	return av_get_bytes_per_sample(m_format.sampleFormat);
}
FFmpeg::Frame AudioBuffer::pop(int frameSize){
	frameSize=std::min(size(),frameSize);
	if(frameSize==0){
		return Frame();
	}
	if(int(data[0].size())<frameSize){
		flushConverter();
	}
	FFmpeg::Frame ret;
    ret->ch_layout=m_format.channelLayout;
    ret->sample_rate=m_format.sampleRate;
    ret->format=m_format.sampleFormat;
    ret->nb_samples=frameSize;
    if(av_frame_get_buffer(*ret,0)<0){
		throw MemoryError("Failed to allocate frame buffer");
	}
	for(int i=0;i<m_format.channelLayout.nb_channels;++i){
		for(int j=0;j<frameSize;++j){
			for(int k=0;k<sampleBytes();++k){
                ret->data[i][j*sampleBytes()+k]=data[i].front()[k];
			}
			data[i].pop_front();
		}
	}
	return ret;
}
vector<FFmpeg::Frame> AudioBuffer::pop(int frameSize,int count){
	vector<FFmpeg::Frame> ret;
	for(int i=0;i<count;++i){
		ret.push_back(pop(frameSize));
	}
	return ret;
}
vector<FFmpeg::Frame> AudioBuffer::flush(int frameSize){
	if(frameSize==0){
		frameSize=1024;
	}
	vector<FFmpeg::Frame> ret;
	while(size()>=frameSize){
		ret.push_back(pop(frameSize));
	}
	ret.push_back(pop(size()));
	return ret;
}

}