#pragma once

#include <chrono>
#include <iostream>

class Timer {
private:
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
	const char* m_Name;
public:
	Timer(const char* name)
		:m_Name(name), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	void stop() {
		auto endTimepoint = std::chrono::high_resolution_clock::now();
		long long start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimepoint).time_since_epoch().count();
		std::cout << m_Name << end - start << "ms" << std::endl;
		m_Stopped = true;
	}

	~Timer() {
		if (!m_Stopped)
			stop();
	}
};

// 一些宏来方便开关计时器和快捷调用计时器
#define ONTIMER 1
#if ONTIMER
#define _TIMER(name) Timer timer##__LINE__(name)
// __FUNCTION__  查看函数名
// windows专用  __FUNCSIG__  查看函数签名
#define TIMER _TIMER(__FUNCSIG__)
#else
#define TIMER
#endif