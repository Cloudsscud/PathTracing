#pragma once

#include<atomic>
#include<thread>

// зда§Ыј
class SpinLock {
public:
	SpinLock() = default;
	SpinLock(const SpinLock&) = delete;
	SpinLock(SpinLock&&) = delete;
	SpinLock& operator=(const SpinLock&&) = delete;
	SpinLock& operator=(SpinLock&&) = delete;

	void acquire() { while (flag.test_and_set(std::memory_order_acquire)) { std::this_thread::yield(); } }
	void release() { flag.clear(std::memory_order_release); }
private:
	std::atomic_flag flag{};
};

class SpinLockGuard {
public:
	SpinLockGuard(SpinLock& spin_lock) :m_spin_lock(spin_lock) {
		m_spin_lock.acquire();
	}
	~SpinLockGuard() {
		m_spin_lock.release();
	}
private:
	SpinLock& m_spin_lock;
};