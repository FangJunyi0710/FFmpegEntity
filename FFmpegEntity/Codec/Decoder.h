#pragma once

#include "Codec.h"

namespace myFFmpeg{

class Decoder:public BasicDecoder{
public:
	Decoder(AVStream* s=nullptr);
	SWAP(Decoder){
		Codec::swap(o);
	}
};

}