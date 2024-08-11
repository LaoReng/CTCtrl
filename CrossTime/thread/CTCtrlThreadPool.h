#pragma once
#include "CTCtrlThread.h"
#include <mutex>
#include <vector>

class CThreadPool
{
public:
	/// <summary>
	/// 有参构造函数
	/// </summary>
	/// <param name="size">指定初始化的线程数</param>
	CThreadPool(size_t size) {
		m_threads.resize(size);
		for (size_t i = 0; i < size; i++) {
			m_threads[i] = new CThread();
		}
	}
	/// <summary>
	/// 析构函数
	/// </summary>
	~CThreadPool() {
		Stop();
		for (size_t i = 0; i < m_threads.size(); i++) {
			delete m_threads[i];
			m_threads[i] = NULL;
		}
		m_threads.clear();
	}
	/// <summary>
	/// 启动线程池
	/// </summary>
	/// <returns>返回值表示启动是否成功 true表示成功 false表示失败</returns>
	bool Invoke() {
		bool ret = true;
		for (size_t i = 0; i < m_threads.size(); i++) {
			if (!m_threads[i]->Start()) { // 线程启动失败
				ret = false;
				break;
			}
		}
		if (ret == false) {
			for (size_t i = 0; i < m_threads.size(); i++) {
				m_threads[i]->Stop();
			}
		}
		return ret;
	}
	/// <summary>
	/// 获取线程池中线程的数量
	/// </summary>
	/// <returns>返回值表示线程池中线程的数量</returns>
	size_t ThreadCnt() const {
		return m_threads.size();
	}

	/// <summary>
	/// 停止线程池运行
	/// </summary>
	void Stop() {
		for (size_t i = 0; i < m_threads.size(); i++) {
			m_threads[i]->Stop();
		}
	}
	/// <summary>
	/// 向线程池投递任务
	/// </summary>
	/// <param name="worker">要投递的任务</param>
	/// <returns>返回值表示投递是否成功， 
	/// 返回值大于0表示第n个线程分配来执行这个任务，返回值为-1表示投递失败
	/// </returns>
	int DispatchWorker(const ThreadWorker& worker) {
		int index = -1;
		m_lock.lock();
		for (size_t i = 0; i < m_threads.size(); i++) {
			if (m_threads[i]->IsIdle()) {
				m_threads[i]->UpdateWorker(worker);
				index = int(i);
				break;
			}
		}
		m_lock.unlock();
		return index;
	}
	/// <summary>
	/// 查看指定线程是否有效
	/// </summary>
	/// <param name="index">查看指定线程的索引</param>
	/// <returns>返回值表示指定线程是否有效， true表示有效 false表示无效</returns>
	bool CheckThreadValid(size_t index) {
		if (index < m_threads.size()) {
			return m_threads[index]->IsValid();
		}
		return false;
	}
private:
	std::mutex            m_lock;    // 用于多线程访问安全
	std::vector<CThread*> m_threads; // 线程的容器
};

