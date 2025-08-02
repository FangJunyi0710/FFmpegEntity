#include "AVInput.h"
#include <QImage>
using namespace myFFmpeg;

int main(){
    AVInput mp3("resource/打上花火.mp3");
    auto& video=mp3.stream(AVMEDIA_TYPE_VIDEO);
    QImage out=VideoFrame(video.decode(1)[0]);
    
    out.save("out/testMeta.png");
    return 0;
}
#include "QtSupport.cpp"