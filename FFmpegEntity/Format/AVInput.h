#pragma once

#include "Stream.h"
#include "AVFormat.h"
#include <array>

namespace myFFmpeg{

class AVInput:public AVFormat{
	std::array<ReadStream*,AVMEDIA_TYPE_NB> streams{};
	AVInput(){}
public:
	AVInput(string url);
	void open(string url);
	SWAP(AVInput){
		AVFormat::swap(o);
		streams.swap(o.streams);
	}

	ReadStream& stream(AVMediaType type){
		if(!streams[type]){throw CodecError("No such stream");}
		return *streams[type];
	}
};

}