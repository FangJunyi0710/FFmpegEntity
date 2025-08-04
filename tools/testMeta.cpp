#include "AVInput.h"
#include "AVOutput.h"
#include "VideoEncoder.h"
#include "AudioEncoder.h"
#include <QImage>
using namespace FFmpeg;

int main(){
	AVInput mp3("resource/打上花火.mp3");
	auto& video=mp3[mp3.index(AVMEDIA_TYPE_VIDEO)];
	QImage out=VideoFrame(video.decode(1)[0]);

	out.save("out/testMeta.png");
	auto metadata = mp3.metadata();
	for (const auto &[key, value] : metadata){
		cDebug(key, ":", value);
	}

	mp3.open("resource/起风了.mpga");
	AVOutput newmp3("out/out.mp3", {
		new VideoEncoder({.id=AV_CODEC_ID_PNG},{480,480,AV_PIX_FMT_RGBA}),
		new AudioEncoder({.id=AV_CODEC_ID_MP3},{AV_CHANNEL_LAYOUT_STEREO,AV_SAMPLE_FMT_FLTP,44100})
	},{},{{{"mimetype","image/jpeg"}}});
	newmp3[0]<<VideoFrame(out).toFrame();
	newmp3[1]<<mp3[mp3.index(AVMEDIA_TYPE_AUDIO)].decode(10000);
	return 0;
}
#include "QtSupport.cpp"