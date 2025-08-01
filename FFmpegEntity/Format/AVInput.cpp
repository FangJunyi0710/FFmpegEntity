#include "AVInput.h"

namespace myFFmpeg{

AVInput::AVInput(string url){
	if(avformat_open_input(&context,url.c_str(),nullptr,nullptr)){
		throw FileError("Cannot open file "+url);
	}
	if(avformat_find_stream_info(context,nullptr)){
		throw FileError("invalid file "+url);
	}
	std::array<vector<Packet>,AVMEDIA_TYPE_NB> packets;
	Packet packet;
	while(!av_read_frame(context, *packet)) {
		packets[context->streams[packet->stream_index]->codecpar->codec_type].push_back(packet);
		packet.unref();
	}
	for(u_int i=0;i<context->nb_streams;++i){
		streams[context->streams[i]->codecpar->codec_type]=new ReadStream(context->streams[i],packets[context->streams[i]->codecpar->codec_type]);
	}
	avformat_close_input(&context);
}

void AVInput::open(string url){
	AVInput tmp(url);
	swap(tmp);
}

// void AVInput::Stream::pushBuffer(const vector<Frame>& frames){
// 	buffer.insert(buffer.end(),frames.begin(),frames.end());
// }
// bool AVInput::Stream::isEmptyBuffer(){
// 	if(!buffer.empty() || !packets.empty()){
// 		return false;
// 	}
// 	decoder.end();
// 	pushBuffer(decoder.receive());
// 	return buffer.empty();
// }


// bool AVInput::Stream::decodeOne(){
// 	while(buffer.empty()){
// 		if(isEmptyBuffer()){
// 			return false;
// 		}
// 		if(!buffer.empty()){
// 			return true;
// 		}
// 		decoder.send({packets.front()});
// 		packets.pop_front();
// 		pushBuffer(decoder.receive());
// 	}
// 	return true;
// }
// vector<Frame> AVInput::Stream::popBuffer(int maxCount){
// 	vector<Frame> ret;
// 	while(decodeOne() && maxCount--){
// 		ret.push_back(buffer.front());
// 		buffer.pop_front();
// 	}
// 	return ret;
// }

}
