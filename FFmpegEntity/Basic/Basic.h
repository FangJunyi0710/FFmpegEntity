#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include "Char033.h"

namespace myFFmpeg{

template<class T>
T lim(const T& a,const T& min,const T& max){
	if(max<a){
		return max;
	}
	if(a<min){
		return min;
	}
	return a;
}
template<class T,class W>
T avg(const T& a,const T& b,const W& wa,const W& wb){
	return (a*wa+b*wb)/(wa+wb);
}
template<class T,class W>
T avg(const T& a,const T& b,const W& wa){
	return avg(a,b,1-wa,wa);
}
using std::max;
using std::string;

using ull=unsigned long long;
using ushort=unsigned short;


#ifdef LOG
// 递归终止函数
void logImpl(std::ostringstream&);

// 递归展开参数包
template <typename T, typename... Args>
void logImpl(std::ostringstream& oss, T&& first, Args&&... args) {
	oss << std::forward<T>(first); // 拼接当前参数
	logImpl(oss, std::forward<Args>(args)...); // 递归处理剩余参数
}

// 日志函数
template <typename... Args>
void logWithContext(const char* file, const char* function, int line, Args&&... args) {
	std::ostringstream oss;
	logImpl(oss, std::forward<Args>(args)...); // 拼接所有参数
	std::clog<<c033::pSkyBlue << "[" << file << ":" << line << " " << function << "] " << c033::pNone << oss.str() << std::endl;
}

// 宏定义简化调用
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define clog(...) logWithContext(__FILENAME__, __func__, __LINE__, __VA_ARGS__)
#else
#define clog(...) ((void)0)
#endif


class FFmpegError:public std::runtime_error{
public:
	FFmpegError(const string& error):std::runtime_error(error){}
};

class FileError:public FFmpegError{
public:
	FileError(const string& error):FFmpegError(error){}
};

class ConvertError:public FFmpegError{
public:
	ConvertError(const string& error):FFmpegError(error){}
};

class CodecError:public ConvertError{
public:
	CodecError(const string& error):ConvertError(error){}
};

class MemoryError:public FFmpegError,std::bad_alloc{
public:
	MemoryError(const string& error):FFmpegError(error){}
};

}

using std::string;
using std::vector;

#define CLONE_(clone,T,pT) \
	pT clone()const{return static_cast<pT>(new T(*this));}
#define CLONE(clone,T,pT) \
	CLONE_(clone,T,pT) \
	T& operator=(const T& o){T t(o);swap(t);return *this;} 
#define COPY(T) \
	T([[maybe_unused]]const T& o)
#define SWAP(T,...) \
	T(T&& o)noexcept:T(__VA_ARGS__){swap(o);} \
	void swap(T&& o)noexcept{swap(o);}\
	T& operator=(T&& o)noexcept{swap(o);return *this;} \
	void swap([[maybe_unused]]T& o)noexcept
	