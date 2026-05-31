#pragma once

#include <chrono>
#include <iostream>
#include <string>

class Timer {
private:
	std::chrono::time_point<std::chrono::steady_clock> m_start;
	bool m_stopped;	// 阻止重复停止
	std::string m_name;
public:
	Timer(const std::string& name)
		:m_name(name), m_stopped(false), m_start(std::chrono::steady_clock::now())
	{}

	void stop() {
		auto end = std::chrono::steady_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start);

		std::cout << m_name << " : " << ms.count() << "ms\n";
		m_stopped = true;
	}

	~Timer() {
		if (!m_stopped)
			stop();
	}
};

// 一些宏来方便开关计时器和快捷调用计时器
#define ONTIMER 1
#if ONTIMER
#define TIMER(name) Timer timer##__LINE__(name)
#else
#define TIMER(name)
#endif