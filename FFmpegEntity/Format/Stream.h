#pragma once

#include "Decoder.h"
#include "Encoder.h"
#include "Log.h"

namespace FFmpeg{

class BasicStream{
	AVRational m_timeBase;
	AVMediaType m_type;
protected:
	BasicStream(AVStream* stream): m_timeBase(stream->time_base), m_type(stream->codecpar->codec_type){}
	~BasicStream()noexcept{}
public:
	SWAP(BasicStream,nullptr){
		std::swap(m_timeBase,o.m_timeBase);
		std::swap(m_type,o.m_type);
	}
	COPY(BasicStream)=delete;

	AVRational timeBase() const{return m_timeBase;}
	AVMediaType type()const{return m_type;}
};

class ReadStream:public BasicStream{
	vector<Packet> m_data;
	using cIT=decltype(m_data)::const_iterator;

	Dictionary m_metadata;

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
	ReadStream(AVStream* stream, const vector<Packet>& data={},const Dictionary& metadata=Dictionary());
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

	double length()const{if(m_data.empty()){return 0;}return double(m_data.back()->pts + m_data.back()->duration)*timeBase();}

	Dictionary metadata()const{return m_metadata;}
};

class WriteStream: public BasicStream{
	Encoder* m_encoder=nullptr;
public:
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

	double length()const{return m_encoder->currentTime();}
};

}