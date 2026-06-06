#pragma once

#include<vector>
#include<thread>
#include<queue>
#include<functional>
//#include <mutex>	// 替换自旋锁
#include"thread/SpinLock.h"


// Task任务类, 通过重写void run()启动
struct Task {
public:
	virtual void run() = 0;
	virtual ~Task() = default;
};

// 线程池, 通过线程来管理Task类，只允许在主线程中添加任务
class ThreadPoll {
public:
	ThreadPoll(size_t thread_count = 0);
	~ThreadPoll();
	
	static void WorkerThread(ThreadPoll* master);	// 线程工作函数
	void wait()const;	// 等待任务列表执行完成

	void addTask(Task* task);	// 向线程池的任务列表尾部添加Task
	Task* getTask();	// 获取头部Task，并从任务列表弹出

	void parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)>& lambda, bool complex = true);	// 并行for执行pixel设置
private:
	std::vector<std::thread> m_threads;	// 线程池本体，存放并管理已经创建的一批线程
	std::queue<Task*> m_tasks;	// 线程池的等待执行任务列表
	// 多个线程对m_tasks资源使用存在竞争
	SpinLock m_tasks_lock;
	//bool m_alive;				// 管理线程池的线程生命周期	改为原子变量
	std::atomic<int> m_alive;
	std::atomic<int> m_tasks_pending_count = 0;	// 还未执行的任务数量,当且仅当添加和执行完之后才变化
};

extern ThreadPoll thread_poll;
