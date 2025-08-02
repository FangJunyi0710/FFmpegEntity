#pragma once
#include "Char033.h"
#include <sstream>

#define LOG_OFF (-5)
#define LOG_FATAL (-4)
#define LOG_ERROR (-3)
#define LOG_WARN (-2)
#define LOG_INFO (-1)
#define LOG_DEBUG 0
#define LOG_TRACE 1
#define LOG_ALL 1
// 若直接定义 -DLOG，默认值为 1（LOG_ALL）

#ifndef LOG
#define LOG LOG_OFF
#else

// 保证 LOG 宏的值合法
static_assert(LOG >= LOG_OFF && LOG <= LOG_ALL, "Invalid value for macro LOG");

namespace LOG_INTERNAL_NAMESPACE{
// 日志级别枚举
enum class LogLevel {
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    TRACE
};

// 递归终止函数
template <typename T>
void logImpl(std::ostringstream& oss, T&& first) {
	oss << std::forward<T>(first); // 拼接当前参数
}

// 递归展开参数包
template <typename T, typename... Args>
void logImpl(std::ostringstream& oss, T&& first, Args&&... args) {
	oss << std::forward<T>(first); // 拼接当前参数
	logImpl(oss, std::forward<Args>(args)...); // 递归处理剩余参数
}

// 日志函数（带级别）
template <typename... Args>
void logWithContext(LogLevel level, const char* file, const char* function, int line, Args&&... args) {
	std::ostringstream oss;
	logImpl(oss, std::forward<Args>(args)...); // 拼接所有参数

	// 根据日志级别设置颜色和前缀
	const char* levelStr = "";
	c033::Pattern color = c033::pNone;
	switch (level) {
	    case LogLevel::FATAL: levelStr = "[FATAL]"; color = c033::pRed; break;
	    case LogLevel::ERROR: levelStr = "[ERROR]"; color = c033::pRed; break;
	    case LogLevel::WARN:  levelStr = "[WARN]";  color = c033::pYellow; break;
	    case LogLevel::INFO:  levelStr = "[INFO]";  color = c033::pGreen; break;
	    case LogLevel::DEBUG: levelStr = "[DEBUG]"; color = c033::pSkyBlue; break;
	    case LogLevel::TRACE: levelStr = "[TRACE]"; color = c033::pBlue; break;
	}

	std::clog << color << levelStr << "[" << file << ":" << line << " " << function << "] " << c033::pNone << oss.str() << std::endl;
}
}

// 宏定义简化调用
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))
#define clog(level, ...) logWithContext(level, __FILENAME__, __func__, __LINE__, __VA_ARGS__)

#endif

#if LOG >= LOG_TRACE
#define cTrace(...) clog(LOG_INTERNAL_NAMESPACE::LogLevel::TRACE,__VA_ARGS__)
#else
#define cTrace(...) ((void)0)
#endif

#if LOG >= LOG_DEBUG
#define cDebug(...) clog(LOG_INTERNAL_NAMESPACE::LogLevel::DEBUG,__VA_ARGS__)
#else
#define cDebug(...) ((void)0)
#endif

#if LOG >= LOG_INFO
#define cInfo(...) clog(LOG_INTERNAL_NAMESPACE::LogLevel::INFO,__VA_ARGS__)
#else
#define cInfo(...) ((void)0)
#endif

#if LOG >= LOG_WARN
#define cWarning(...) clog(LOG_INTERNAL_NAMESPACE::LogLevel::WARN,__VA_ARGS__)
#else
#define cWarning(...) ((void)0)
#endif

#if LOG >= LOG_ERROR
#define cError(...) clog(LOG_INTERNAL_NAMESPACE::LogLevel::ERROR,__VA_ARGS__)
#else
#define cError(...) ((void)0)
#endif

#if LOG >= LOG_FATAL
#define cFatal(...) clog(LOG_INTERNAL_NAMESPACE::LogLevel::FATAL,__VA_ARGS__)
#else
#define cFatal(...) ((void)0)
#endif

#undef LOG
#undef LOG_OFF
#undef LOG_FATAL
#undef LOG_ERROR
#undef LOG_WARN
#undef LOG_INFO
#undef LOG_DEBUG
#undef LOG_TRACE
#undef LOG_ALL
