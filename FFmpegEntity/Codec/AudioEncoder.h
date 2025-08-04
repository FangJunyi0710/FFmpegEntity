#pragma once

#include "Encoder.h"

namespace FFmpeg{

class AudioEncoder:public Encoder{
protected:
	AudioBuffer buffer;
	AudioEncoder();
	double step()const override;
	vector<Frame> convertFormat(const vector<Frame>& source)override;
	vector<Frame> flushBuffer()override;
public:
	AudioEncoder(const Encoder::Params&,AudioFormat format);
	~AudioEncoder()noexcept{}
	SWAP(AudioEncoder){
		Encoder::swap(o);
	}
};

}