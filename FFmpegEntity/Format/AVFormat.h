#pragma once

#include "Frame.h"

namespace FFmpeg{

using std::string;

class AVFormat{
protected:
	AVFormatContext* context=nullptr;
	AVFormat(){}
	~AVFormat()noexcept{
		avformat_free_context(context);
	}
public:
	SWAP(AVFormat){
		std::swap(context,o.context);
	}
	COPY(AVFormat)=delete;
};

}