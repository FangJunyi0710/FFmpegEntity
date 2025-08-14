#include "AVInput.h"

namespace FFmpeg{

AVInput::AVInput(string url){
	if(avformat_open_input(&context,url.c_str(),nullptr,nullptr)){
		throw FileError("Cannot open file "+url);
	}
	if(avformat_find_stream_info(context,nullptr)){
		throw FileError("invalid file "+url);
	}
	streams.resize(context->nb_streams);
	std::vector<vector<Packet>> packets(streams.size());
	Packet packet;
	while(!av_read_frame(context, *packet)) {
		packets[packet->stream_index].push_back(packet);
		packet.unref();
	}
	for(u_int i=0;i<context->nb_streams;++i){
		streams[i]=new ReadStream(context->streams[i],packets[i],readAVDictionary(context->streams[i]->metadata));
		m_streamIndex[streams[i]->type()].push_back(i);
	}
	m_metadata=readAVDictionary(context->metadata);
	avformat_close_input(&context);
}

void AVInput::open(string url){
	AVInput tmp(url);
	swap(tmp);
}

}
