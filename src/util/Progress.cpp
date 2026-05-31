#include "util/Progress.h"
#include <iostream>

Progress::Progress(size_t total, size_t step)
	:m_total(total), m_current(0), m_percent(0), m_pre_percent(0), m_step(step)
{
	std::cout << "0%" << std::endl;
}

void Progress::update(size_t count) {
	SpinLockGuard guard(m_spinlock);

	m_current += count;
	m_percent = 100*static_cast<float>(m_current) / static_cast<float>(m_total);
	if (m_percent - m_pre_percent >= m_step || m_percent == 100) {
		m_pre_percent = m_percent;
		std::cout << m_percent << "%" << std::endl;
	}
}