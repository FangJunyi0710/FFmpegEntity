#pragma once

#include "Color.h"

extern "C"{
#include <libavformat/avformat.h>
}

namespace FFmpeg{

class Packet{
	AVPacket* m_data=nullptr;
public:
	Packet();
	~Packet()noexcept;
	SWAP(Packet){
		std::swap(m_data,o.m_data);
	}
	COPY(Packet):m_data(av_packet_clone(o.m_data)){
		if(!m_data){
			m_data=av_packet_alloc();
		}
		if(!m_data){
			throw MemoryError("Failed to allocate AVPacket");
		}
	}
    CLONE(clone,Packet,Packet*)
    void unref()const;

    AVPacket* operator*(){return m_data;}
    const AVPacket* operator*()const{return m_data;}
    AVPacket* operator->(){return m_data;}
    const AVPacket* operator->()const{return m_data;}
};

}