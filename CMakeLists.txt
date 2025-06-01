cmake_minimum_required(VERSION 3.14)

project(VideoGenerateLANGUAGES CXX)

add_subdirectory(FFmpegEntity)

set(SOURCES
    
    main.cpp
)

add_executable(VideoGenerate main.cpp)
target_link_libraries(VideoGenerate PRIVATE FFmpegEntity)
