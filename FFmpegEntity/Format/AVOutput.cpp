#include "AVOutput.h"

namespace FFmpeg{

AVOutput::AVOutput(string filename,const vector<Encoder*>& arg_encoders,const Dictionary& metadata,const vector<Dictionary>& streamMetadatas){
	cDebug("");
	if(avformat_alloc_output_context2(&context,nullptr,nullptr,filename.c_str())<0){
		throw FFmpegError("init failed");
	}
	if(avio_open(&context->pb,filename.c_str(),AVIO_FLAG_WRITE)<0){
		throw FileError("Cannot open file "+filename);
	}

	std::vector<AVStream*> tempStreams(arg_encoders.size());
	for(ull i=0;i<tempStreams.size();++i){
		tempStreams[i]=avformat_new_stream(context,nullptr);
		arg_encoders[i]->setStream(tempStreams[i]);
		if(streamMetadatas.size()>ull(i)){
			writeAVDictionary(tempStreams[i]->metadata,streamMetadatas[i]);
		}
	}
	writeAVDictionary(context->metadata,metadata);

	if(avformat_write_header(context,nullptr)<0){
		throw FFmpegError("Writing header failed");
	}
	streams.resize(tempStreams.size());
	for(ull i=0;i<tempStreams.size();++i){
		streams[i]=new WriteStream(tempStreams[i],arg_encoders[i]);
	}
}
void AVOutput::flush(){
	vector<Packet> buffer;
	for(const auto& each:streams){
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
	if(streams.empty()){
		return;
	}
	for(const auto& each:streams){
		each->closeEncoder();
	}
	flush();
	av_write_trailer(context);
	avio_close(context->pb);
	for(auto& each:streams){
		delete each;
	}
	streams.clear();
}
AVOutput::~AVOutput()noexcept{
	close();
}

}