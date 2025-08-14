#include "VideoEncoder.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>

namespace FFmpeg {

using std::min;

class VideoEncoder::Handler {
	std::mutex lock;
	std::condition_variable condition;
	bool stop = false;
	vector<std::thread> th;
	std::atomic<int> working = 0;

	deque<Frame> buffer;
	vector<Frame> converted;
	VideoFormat outputFormat;

	ull indexIn=0,indexOut=0;
public:
	Handler(VideoFormat oFormat): outputFormat(oFormat) {
		th = vector<std::thread>(std::thread::hardware_concurrency());
		for(auto& t : th) {
			t = std::thread([&]() {
				while(!stop) {
					std::unique_lock<std::mutex> locker(lock);
					condition.wait(locker, [ = ]() {return !buffer.empty() || stop;});
					if(stop) {
						break;
					}
					auto cur = buffer[0];
					++working;
					buffer.pop_front();
					locker.unlock();

					auto res = Swscale(cur, outputFormat).scale(cur);
					res->pts = cur->pts;

					locker.lock();
					converted.push_back(res);
					--working;
				}
			});
		}
	}
	void push(const vector<Frame>& source) {
		std::lock_guard<std::mutex> locker(lock);
		for(const auto& i : source) {
			auto tmp=i;
			tmp->pts=++indexIn;
			if(tmp == outputFormat) {
				converted.push_back(tmp);
				continue;
			}
			buffer.push_back(tmp);
		}

		if(buffer.size() >= std::thread::hardware_concurrency()) {
			condition.notify_all();
		} else {
			condition.notify_one();
		}
	}
	vector<Frame> pop() {
		vector<Frame> ret;
		std::unique_lock<std::mutex> locker(lock);
		ret.swap(converted);
		locker.unlock();
		std::sort(ret.begin(),ret.end(),[=](const Frame& a,const Frame& b){return a->pts<b->pts;});
		for(ull i=0;i<ret.size();++i){
			if(ull(ret[i]->pts)!=indexOut+i+1){
				locker.lock();
				converted.insert(converted.end(),ret.begin()+i,ret.end());
				locker.unlock();
				ret.erase(ret.begin()+i,ret.end());
				break;
			}
		}
		if(!ret.empty()){
			indexOut=ret.back()->pts;
		}
		return ret;
	}
	vector<Frame> flush() {
		while(working || !buffer.empty());
		return pop();
	}
	~Handler()noexcept {
		stop = true;
		condition.notify_all();
		for(auto& t : th) {
			t.join();
		}
	}
};

double VideoEncoder::step()const {
	return 1.0 / fps;
}
vector<Frame> VideoEncoder::convertFormat(const vector<Frame>& source) {
	handler->push(source);
	return handler->pop();
}
vector<Frame> VideoEncoder::flushBuffer() {
	return handler->flush();
}
VideoEncoder::VideoEncoder(const Encoder::Params& p, VideoFormat format, int fps_):Encoder(p), handler(new Handler(format)), fps(fps_){
	context->pix_fmt = format.pix_fmt;
	context->width = format.width;
	context->height = format.height;

	context->gop_size = fps * 5;   // I帧间隔
	context->max_b_frames = 4; // B帧 ( I BBB P BBB P BBB P BBB P BBB I )

	context->time_base = {1, fps};
	context->framerate = {fps, 1};
}
VideoEncoder::~VideoEncoder() noexcept{
    delete handler;
}
}
