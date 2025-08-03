#pragma once

#include "Stream.h"
#include "AVFormat.h"

namespace myFFmpeg{

class AVOutput:public AVFormat{
	std::array<WriteStream*,AVMEDIA_TYPE_NB> streams{};
	AVOutput(){}
public:
	AVOutput(string filename,const vector<Encoder*>& arg_encoders,const Dictionary& metadata={},const vector<Dictionary>& streamMetadatas={});
	SWAP(AVOutput){
		AVFormat::swap(o);
		streams.swap(o.streams);
	}
	WriteStream& stream(AVMediaType type){
		if(!streams[type]){throw CodecError("No such stream");}
		return *streams[type];
	}
	void flush();
	void close();
	~AVOutput()noexcept;
};

}