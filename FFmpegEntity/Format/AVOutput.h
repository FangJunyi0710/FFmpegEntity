#pragma once

#include "Stream.h"
#include "AVFormat.h"

namespace myFFmpeg{

class AVOutput:public AVFormat{
	vector<WriteStream*> streams;
	AVOutput(){}
public:
	AVOutput(string filename,const vector<Encoder*>& arg_encoders,const Dictionary& metadata={},const vector<Dictionary>& streamMetadatas={});
	SWAP(AVOutput){
		AVFormat::swap(o);
		streams.swap(o.streams);
	}
	WriteStream& operator[](int index){
		return *streams[index];
	}
	void flush();
	void close();
	~AVOutput()noexcept;
};

}