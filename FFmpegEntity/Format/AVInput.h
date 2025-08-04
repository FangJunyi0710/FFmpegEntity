#pragma once

#include "Stream.h"
#include "AVFormat.h"
#include <array>

namespace FFmpeg{

class AVInput:public AVFormat{
	vector<ReadStream*> streams;
	Dictionary m_metadata;
	std::array<vector<int>,AVMEDIA_TYPE_NB> m_streamIndex;
	AVInput(){}
public:
	AVInput(string url);
	void open(string url);
	SWAP(AVInput){
		AVFormat::swap(o);
		streams.swap(o.streams);
		m_metadata.swap(o.m_metadata);
		m_streamIndex.swap(o.m_streamIndex);
	}

	ReadStream& operator[](int index){
		if(index>=int(streams.size()) || index<0){
			throw FFmpegError("stream index out of range");
		}
		return *streams[index];
	}
	int index(AVMediaType type,int index=0){
		if(int(m_streamIndex[type].size())<=index){
			return -1;
		}
		return m_streamIndex[type][index];
	}
	Dictionary metadata()const{return m_metadata;}
};

}