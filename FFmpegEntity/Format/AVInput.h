#pragma once

#include "Stream.h"
#include "AVFormat.h"
#include <array>

namespace myFFmpeg{

class AVInput:public AVFormat{
	std::array<ReadStream*,AVMEDIA_TYPE_NB> streams{};
	Dictionary m_metadata;
	AVInput(){}
public:
	AVInput(string url);
	void open(string url);
	SWAP(AVInput){
		AVFormat::swap(o);
		streams.swap(o.streams);
		m_metadata.swap(o.m_metadata);
	}

	ReadStream& stream(AVMediaType type){
		if(!streams[type]){throw CodecError("No such stream");}
		return *streams[type];
	}
	Dictionary metadata()const{return m_metadata;}
};

}