#include "Encoder.h"

namespace myFFmpeg{

void Encoder::setpts(vector<Frame>& source){
	for(auto& each:source){
		each->pts=curTime/m_stream->time_base;
		each->pict_type=AV_PICTURE_TYPE_NONE;// 不指定帧类型
		curTime+=step();
	}
}
vector<Frame> Encoder::pretreat(const vector<Frame>& source){
	auto ret=convertFormat(source);
	setpts(ret);
	return ret;
}
vector<Packet> Encoder::aftertreat(const vector<Packet>& products)const{
	auto ret=products;
	for(auto& each:ret){
		each->stream_index=m_stream->index;
	}
	return ret;
}
Encoder::Encoder(const Params& p){
	codec=avcodec_find_encoder(p.id);
	if(!codec){
		throw CodecError("No available encoder");
	}
	context=avcodec_alloc_context3(codec);
	context->codec_id = p.id;
	context->codec_type = avcodec_get_type(p.id);
	context->bit_rate = p.bit_rate;
	writeAVDictionary(options,p.opts);
}
AVMediaType Encoder::type()const{
	return context->codec_type;
}
void Encoder::configure(){
	readStream();
	open();
}

}
