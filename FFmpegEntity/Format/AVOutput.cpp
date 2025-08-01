#include "AVOutput.h"

namespace myFFmpeg{

AVOutput::AVOutput(string filename,const vector<Encoder*>& arg_encoders){
	if(avformat_alloc_output_context2(&context,nullptr,nullptr,filename.c_str())<0){
		throw FFmpegError("init failed");
	}
	if(avio_open(&context->pb,filename.c_str(),AVIO_FLAG_WRITE)<0){
		throw FileError("Cannot open file "+filename);
	}

	std::array<std::pair<Encoder*,AVStream*>,AVMEDIA_TYPE_NB> tempStreams{};
	for(auto& each:arg_encoders){
		delete tempStreams[each->type()].first;
		tempStreams[each->type()]={each,nullptr};
	}
	for(auto& each:tempStreams){
		if(!each.first){
			continue;
		}
		each.second=avformat_new_stream(context,nullptr);
		each.first->setStream(each.second);
	}

	if(avformat_write_header(context,nullptr)<0){
		throw FFmpegError("Writing header failed");
	}
	for(auto& each:tempStreams){
		if(!each.first){
			continue;
		}
		streams[each.first->type()]=new WriteStream(each.second,each.first);
	}
}
void AVOutput::flush(){
	vector<Packet> buffer;
	for(const auto& each:streams){
		if(!each){
			continue;
		}
		auto tmp=each->flush();
		buffer.insert(buffer.end(),tmp.begin(),tmp.end());
	}
	sort(buffer.begin(),buffer.end(),[=](const Packet& a,const Packet& b){
		return a->dts<b->dts;
	});
	for(auto each:buffer){
		av_interleaved_write_frame(context,*each);
	}
}
void AVOutput::close(){
	bool hasEncoder=false;
	for(const auto& each:streams){
		if(!each){
			continue;
		}
		each->closeEncoder();
		hasEncoder=true;
	}
	if(!hasEncoder){
		return;
	}
	flush();
	av_write_trailer(context);
	avio_close(context->pb);
	for(auto& each:streams){
		delete each;
		each=nullptr;
	}
}
AVOutput::~AVOutput()noexcept{
	close();
}

}