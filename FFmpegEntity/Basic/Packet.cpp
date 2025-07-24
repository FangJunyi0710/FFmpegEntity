#include "Packet.h"
#include "Basic.h"
#include <cassert>

namespace myFFmpeg{

Packet::Packet():m_data(av_packet_alloc()){
    if(!m_data){
        throw MemoryError("Failed to allocate packet");
    }
}
Packet::~Packet()noexcept{
	av_packet_free(&m_data);
}
void Packet::unref()const{
    av_packet_unref(m_data);
}

}