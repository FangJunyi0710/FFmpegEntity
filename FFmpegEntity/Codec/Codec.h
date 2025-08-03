#pragma once

#include "Frame.h"
#include "Packet.h"
#include <map>

extern "C"{
#include <libavcodec/avcodec.h>
}

namespace myFFmpeg{

double operator*(int64_t tick,AVRational timeBase);
double operator*(AVRational timeBase,int64_t tick);
int64_t operator/(double time,AVRational timeBase);

using Dictionary=std::map<string,string>;
void writeAVDictionary(AVDictionary*& options,const Dictionary& opts);
Dictionary readAVDictionary(AVDictionary* dict);


/**
 * Codec:
 *   send():
 *     pretreat() 进入子类缓冲区 ==> send_() 进入AVCodec内部缓冲区 & receive_() ==> 进入缓冲区
 *   receive():
 *     取出缓冲区 ==> aftertreat()
 *   end():
 *     flushBuffer() 清空子类缓冲区 ==> send_(nullptr) 清空AVCodec内部缓冲区 ==> receive_() 进入缓冲区
 *     调用后不能再调用 send(), 应调用 receive() 取出剩余缓冲区的内容
 *   reset():
 *     和 end() 功能类似，都会清空AVCodec内部缓冲区，但不会禁用编解码器。
 * 
 */
template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(**fromT())>::type*),int(*receive_)(AVCodecContext*,decltype(*toT()))>
class Codec{
	deque<toT> outputBuffer;
protected:
	const AVCodec* codec=nullptr;
	AVCodecContext* context=nullptr;
	AVDictionary* options=nullptr;
	AVStream* m_stream=nullptr;

	void open();
	void readStream();
	void writeStream()const;
	
	virtual vector<fromT> pretreat(const vector<fromT>& source);
	virtual vector<toT> aftertreat(const vector<toT>& products)const;
	virtual vector<fromT> flushBuffer();

	Codec(){}
	virtual ~Codec()noexcept;
public:
	SWAP(Codec){
		std::swap(codec,o.codec);
		std::swap(context,o.context);
	}
	COPY(Codec)=delete;

	AVMediaType type()const{return codec->type;}

	const AVStream* stream();
	void setStream(AVStream* s);
	
	void send(const vector<fromT>& source);
	vector<toT> receive(int maxCount=-1);
	void end();

	void reset();
};

using BasicEncoder=Codec<Frame,Packet,avcodec_send_frame,avcodec_receive_packet>;
using BasicDecoder=Codec<Packet,Frame,avcodec_send_packet,avcodec_receive_frame>;

}
