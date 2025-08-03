#pragma once

#include "Codec.h"

namespace myFFmpeg{

/**
 * 用法：
 *   只能使用子类。
 * Encoder* encoder=new SomeEncoder(args...);
 * encoder.setStream(stream);
 * encoder.configure();
 */
class Encoder:public BasicEncoder{
	double curTime=0;
	void setpts(vector<Frame>& source);
protected:
	Encoder():Encoder(Params()){}
	virtual double step()const=0;
	virtual vector<Frame> convertFormat(const vector<Frame>& source)=0;
	vector<Frame> pretreat(const vector<Frame>& source)override;
	vector<Packet> aftertreat(const vector<Packet>& products)const override;
public:
	struct Params{
		AVCodecID id=AV_CODEC_ID_NONE;
		int64_t bit_rate=0;
		std::map<string,string> opts={};
	};
	Encoder(const Params& p);
	~Encoder()noexcept{}
	SWAP(Encoder){
		Codec::swap(o);
		std::swap(curTime,o.curTime);
	}
	COPY(Encoder)=delete;
	AVMediaType type()const;
	void configure();
};

}