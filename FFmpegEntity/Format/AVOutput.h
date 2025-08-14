#pragma once

#include "Stream.h"
#include "AVFormat.h"

namespace FFmpeg{

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
		if(index>=int(streams.size()) || index<0){
			throw FFmpegError("stream index out of range");
		}
		return *streams[index];
	}
	void flush();
	void close();
	~AVOutput()noexcept;
};

}