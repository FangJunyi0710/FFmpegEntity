#pragma once

#include "Decoder.h"
#include "Encoder.h"

namespace myFFmpeg{

/**
 * 这是一个自动管理编解码的封装类。
 * 使用指针机制，读（解码）与写（编码）都基于当前指针位置。
 * 可以根据时间戳移动指针。
 */
class BasicStream{
	// AVStream* m_stream;
	AVRational m_timeBase;
	AVMediaType m_type;
protected:
	BasicStream(AVStream* stream): m_timeBase(stream->time_base), m_type(stream->codecpar->codec_type){}
	~BasicStream()noexcept{}
public:
	SWAP(BasicStream,nullptr){
		// std::swap(m_stream,o.m_stream);
	}
	COPY(BasicStream)=delete;

	AVRational timeBase() const{return m_timeBase;}
	AVMediaType type()const{return m_type;}
};

class ReadStream:public BasicStream{
	vector<Packet> m_data;
	using cIT=decltype(m_data)::const_iterator;

	std::map<int64_t,cIT> ptsMap;
	Decoder* m_decoder=nullptr;
	deque<Frame> cache;

	cIT point;

	cIT map(const cIT& x)const{return x;}
	cIT map(double time)const;

	void sortData();
	void buildPTSMap();
	void movePoint(const cIT& newPoint);

	cIT seekKeyFrame(cIT pos)const;
	vector<Frame> decode(const cIT& end);
	void flushDecoder();
public:
	// 只读
	ReadStream(AVStream* stream, const vector<Packet>& data={});
	SWAP(ReadStream,nullptr){
		BasicStream::swap(o);
		std::swap(m_decoder,o.m_decoder);
		std::swap(m_data,o.m_data);
		std::swap(point,o.point);
	}
	~ReadStream()noexcept;

	vector<Frame> decode(double len);

	double currentTime() const;
	template<class T>
	void moveTo(T it){movePoint(map(it));}
};

class WriteStream: public BasicStream{
	Encoder* m_encoder=nullptr;
public:
	// 只写
	WriteStream(AVStream* stream, Encoder* encoder);
	SWAP(WriteStream,nullptr,nullptr){
		BasicStream::swap(o);
		std::swap(m_encoder,o.m_encoder);
	}
	~WriteStream()noexcept;

	void encode(const vector<Frame>& frames);
	void encode(const Frame& frame);
	template<class T>
	WriteStream& operator<<(const T& frame){encode(frame);return *this;}
	
	vector<Packet> flush()const;
	void closeEncoder() const;
};

}