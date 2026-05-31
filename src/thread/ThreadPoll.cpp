#include "thread/ThreadPoll.h"

void ThreadPoll::WorkerThread(ThreadPoll* master) {
	// 只要线程池生命周期存在，线程便一直处理Task
	while (master->m_alive == 1) {
		Task* task = master->getTask(); // 获取任务时尚未执行任务，所以待执行任务数不变
		if (task != nullptr) {
			task->run();
			// 执行完之后才允许待执行任务数量减一
			master->m_tasks_pending_count--;
		}
		else {
			// 无任务时，此子线程允许放弃CPU时间片轮转
			std::this_thread::yield();
		}
	}
}

ThreadPoll::ThreadPoll(size_t thread_count) {
	//m_alive = true;	// 创建线程池时生命周期设为存活
	m_alive = 1;

	if (thread_count == 0) {
		// 未显式要求线程数量，则CPU硬件默认数量
		thread_count = std::thread::hardware_concurrency();
	}
	for (size_t i = 0; i < thread_count; ++i) {
		m_threads.push_back(std::thread(WorkerThread, this));	// 进程使用static占位方法
	}
}

void ThreadPoll::wait() const {
	// 任务全部执行完之后才允许线程池销毁，防止子线程尚未执行完任务便被销毁
	while (m_tasks_pending_count > 0) {
		// 在等待任务完成的过程中，本线程也在CPU调度，将本线程交给OS高效管理
		std::this_thread::yield();
		// 缺点：只允许在主线程中添加任务
		// 后续优化：通过条件变量，直接让线程休眠，直到任务列表为空
	}
}


ThreadPoll::~ThreadPoll() {
	wait();	// 任务列表为空时才允许线程池销毁
	//m_alive = false;	// 销毁线程池时生命周期设为死亡
	m_alive = 0;	// 存在风险：子线程此时添加任务导致错误流程

	// 等待所有线程执行完毕后清理
	for (auto& thread : m_threads) {
		thread.join();
	}
	m_threads.clear();
}

void ThreadPoll::addTask(Task* task) {
	//std::lock_guard<std::mutex> guard(m_tasks_lock);	// mutex管理锁m_tasks_lock 弃用，转为自旋锁
	SpinLockGuard guard(m_tasks_lock);
	m_tasks_pending_count++;
	m_tasks.push(task);

}

Task* ThreadPoll::getTask() {
	//std::lock_guard<std::mutex> guard(m_tasks_lock);	// mutex管理锁m_tasks_lock 弃用，转为自旋锁
	SpinLockGuard guard(m_tasks_lock);

	if (m_tasks.empty()) {
		return nullptr;
	}
	Task* task = m_tasks.front();
	m_tasks.pop();
	return task;
}

class ParallelForTask :public Task{
public:
	ParallelForTask(size_t x, size_t y, const std::function<void(size_t, size_t)>& lambda)
		:m_x(x), m_y(y), m_lambda(lambda)
	{}

	void run() override {
		m_lambda(m_x, m_y);
	}

private:
	std::function<void(size_t, size_t)> m_lambda;
	size_t m_x, m_y;
};

void ThreadPoll::parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)>& lambda) {
	SpinLockGuard guard(m_tasks_lock);

	for (size_t x = 0; x < width; ++x) {
		for (size_t y = 0; y < height; ++y) {
			m_tasks_pending_count++;
			m_tasks.push(new ParallelForTask(x, y, lambda));
		}
	}

}
