#include "Decoder.h"
#include <iostream>

namespace FFmpeg{

Decoder::Decoder(AVStream* s){
	if(!s){
		return;
	}
	setStream(s);
	codec=avcodec_find_decoder(m_stream->codecpar->codec_id);
	if(!codec){
		throw CodecError("No available decoder");
	}
	context=avcodec_alloc_context3(codec);
	readStream();
	open();
}

}
