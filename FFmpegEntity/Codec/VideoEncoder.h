#pragma once

#include "Encoder.h"

namespace FFmpeg{

class VideoEncoder:public Encoder{
	class Handler;
	Handler* handler;
protected:
	int fps;
	VideoEncoder(){}
	double step()const override;
	vector<Frame> convertFormat(const vector<Frame>& source)override;
	vector<Frame> flushBuffer()override;
public:
	VideoEncoder(const Encoder::Params&,VideoFormat format,int fps_=30);
	~VideoEncoder()noexcept;
	SWAP(VideoEncoder){
		Encoder::swap(o);
		std::swap(fps,o.fps);
		std::swap(handler,o.handler);
	}
};

}