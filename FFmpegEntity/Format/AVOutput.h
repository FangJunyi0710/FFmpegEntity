#pragma once

#include "Stream.h"
#include "AVFormat.h"

namespace myFFmpeg{

class AVOutput:public AVFormat{
	std::array<WriteStream*,AVMEDIA_TYPE_NB> streams{};
public:
	AVOutput(string filename,const vector<Encoder*>& arg_encoders);
	SWAP(AVOutput,"",{}){
		AVFormat::swap(o);
		streams.swap(o.streams);
	}
	WriteStream& stream(AVMediaType type){return *streams[type];}
	void flush();
	void close();
	~AVOutput()noexcept;
};

}