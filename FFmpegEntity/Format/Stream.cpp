#include "Stream.h"

namespace myFFmpeg {

// ==================== BasicStream ====================


// ==================== ReadStream ====================
void ReadStream::sortData() {
	sort(m_data.begin(), m_data.end(), [=](const Packet& a, const Packet& b) {
		if (a->dts != b->dts) {
			return a->dts < b->dts; // 优先按 DTS 排序
		}
		return a->pts < b->pts;     // 其次按 PTS 排序
	});
}

void ReadStream::buildPTSMap(){
	ptsMap.clear();
	for(auto it=m_data.begin();it!=m_data.end();++it){
		ptsMap[(*it)->pts]=it;
	}
	ptsMap[-0x8000000000000000]=m_data.begin();
	ptsMap[0x7fffffffffffffff]=m_data.end();
}

void ReadStream::movePoint(const cIT& newPoint) {
	if (newPoint == point) {
		return;
	}
	flushDecoder();
	point = newPoint;
}

ReadStream::cIT ReadStream::seekKeyFrame(cIT pos) const {
	if (m_decoder->type() != AVMEDIA_TYPE_VIDEO) {
		return pos; // 非视频流无需查找关键帧
	}
	for(cIT i=pos;i!=m_data.begin();--i){
		if((*i)->flags & AV_PKT_FLAG_KEY) {
			return i;
		}
	}
	return m_data.begin();
}

vector<Frame> ReadStream::decode(const cIT &end) {
	// 1. 发送从当前点到结束点的数据包到解码器
	m_decoder->send(vector<Packet>(seekKeyFrame(point), end));

	// 2. 接收解码后的帧，并按 PTS 排序
	vector<Frame> frames = m_decoder->receive();
	std::sort(frames.begin(), frames.end(), [](const Frame& a, const Frame& b) {
		return a->pts < b->pts;
	});

	// 3. 记录当前时间范围
	int64_t ptsBegin = 0x7fffffffffffffff, ptsEnd = 0x7fffffffffffffff;
	if(point!=m_data.end()){
		ptsBegin=(*point)->pts;
	}
	if(end!=m_data.end()){
		ptsEnd = (*end)->pts;
	}
	point = end; // 更新当前点

	// 4. 如果是视频流，处理缓存和合并帧
	if (m_decoder->type() == AVMEDIA_TYPE_VIDEO) {
		// 4.1 定位当前时间范围内的帧
		auto lower = std::lower_bound(frames.begin(), frames.end(), ptsBegin,
			[](const Frame& f, int64_t t) { return f->pts < t; });
		auto upper = std::lower_bound(lower, frames.end(), ptsEnd,
			[](const Frame& f, int64_t t) { return f->pts < t; });

		// 4.2 缓存超出当前范围的帧
		cache.insert(cache.end(), upper, frames.end());

		// 4.3 处理缓存中的帧
		auto cacheLower = std::lower_bound(cache.begin(), cache.end(), ptsBegin,
			[](const Frame& f, int64_t t) { return f->pts < t; });
		auto cacheUpper = std::lower_bound(cacheLower, cache.end(), ptsEnd,
			[](const Frame& f, int64_t t) { return f->pts < t; });
		// 4.4 合并当前帧和缓存帧
		vector<Frame> ret((upper - lower) + (cacheUpper - cacheLower));
		std::merge(lower, upper, cacheLower, cacheUpper, ret.begin(),
			[](const Frame& a, const Frame& b) { return a->pts < b->pts; });
		// 4.5 更新缓存
		cache.erase(cache.begin(), cacheUpper); // 移除已处理的帧
		return ret;
	}

	// 5. 非视频流直接返回解码帧
	return frames;
}

ReadStream::cIT ReadStream::map(double time)const{
	return ptsMap.lower_bound(time/timeBase())->second;
}

ReadStream::ReadStream(AVStream* stream, const vector<Packet>& data, const Dictionary& metadata)
	: BasicStream(stream),m_data(data.begin(),data.end()),m_metadata(metadata), m_decoder(new Decoder(stream)), point(m_data.begin()) {
	sortData();      // 按 DTS 排序
	buildPTSMap();   // 初始化 PTS 映射
}

double ReadStream::currentTime() const {
	if(m_data.empty()){
		return 0;
	}
	if(point==m_data.end()){
		return (*m_data.rbegin())->pts*timeBase();
	}
	return (*point)->pts * timeBase();
}

vector<Frame> ReadStream::decode(double len) {
	return decode(map(currentTime() + len));
}

void ReadStream::flushDecoder(){
	m_decoder->reset();

	auto tmp=m_decoder->receive();
	cache.insert(cache.end(), tmp.begin(), tmp.end());
	if(point!=m_data.end()){
		cache.erase(cache.begin(), std::lower_bound(cache.begin(), cache.end(), (*point)->pts,
			[](const Frame& f, int64_t t) { return f->pts < t; })); 
	}
}

ReadStream::~ReadStream() noexcept{
	delete m_decoder;
}

// ==================== WriteStream ====================
WriteStream::WriteStream(AVStream* stream, Encoder* encoder): BasicStream(stream), m_encoder(encoder) {
	if (encoder) {
		encoder->setStream(stream);
		encoder->configure();
	}
}

WriteStream::~WriteStream() noexcept{
	delete m_encoder;
}

void WriteStream::encode(const vector<Frame>& frames){
	m_encoder->send(frames);
}

void WriteStream::encode(const Frame& frame){
	encode(vector<Frame>{frame});
}

vector<Packet> WriteStream::flush()const{
	return m_encoder->receive();
}

void WriteStream::closeEncoder() const{
	m_encoder->end();
}
}