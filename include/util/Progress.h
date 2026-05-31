#pragma once

#include <thread/SpinLock.h>

class Progress {
private:
	size_t m_total, m_current;
	int m_percent, m_pre_percent, m_step;
	SpinLock m_spinlock;
public:
	Progress(size_t total, size_t step = 1);
	void update(size_t count);
};