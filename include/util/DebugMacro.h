#pragma once

// 调试加速结构的详细信息时使用，否则注释即可
#define DEBUG_INFO

#ifdef DEBUG_INFO
#define DEBUG_LINE(...) __VA_ARGS__ ;
#else
#define DEBUG_LINE(...)
#endif