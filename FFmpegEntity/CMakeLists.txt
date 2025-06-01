cmake_minimum_required(VERSION 3.10)

project(FFmpegEntity)

# 所有源文件
set(SOURCES
    Basic/Basic.cpp
    Basic/Color.cpp
    Basic/Formats.cpp
    Basic/Frame.cpp
    Basic/Packet.cpp
    Codec/Codec.cpp
    Codec/Decoder.cpp
    Codec/Encoder.cpp
    Codec/VideoEncoder.cpp
    Codec/AudioEncoder.cpp
    Format/AVFormat.cpp
    Format/AVInput.cpp
    Format/AVOutput.cpp

    Basic/Basic.h
    Basic/Color.h
    Basic/Formats.h
    Basic/Frame.h
    Basic/Packet.h
    Codec/Codec.h
    Codec/Decoder.h
    Codec/Encoder.h
    Codec/VideoEncoder.h
    Codec/AudioEncoder.h
    Format/AVFormat.h
    Format/AVInput.h
    Format/AVOutput.h
)

add_library(FFmpegEntity ${SOURCES})

target_include_directories(FFmpegEntity PUBLIC
	Format
	Codec
	Basic
)

# 查找 pkg-config
find_package(PkgConfig REQUIRED)

# 通过 pkg-config 获取 FFmpeg 的编译和链接参数
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
    libavfilter
    libavdevice
    libswresample
)

# 链接时直接使用 PkgConfig 生成的接口
target_link_libraries(FFmpegEntity PRIVATE PkgConfig::FFMPEG)

# 在 CMakeLists.txt 中添加 Sanitizers
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()
