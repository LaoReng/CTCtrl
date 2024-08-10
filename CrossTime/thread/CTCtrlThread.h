#pragma once
#include <iostream>
#include "CTCtrlFunction.h"
// #include <Windows.h> // windows
#include <atomic>
// #include <process.h>  // 线程函数存放位置 // windows

#ifndef WIN32
#include <unistd.h>
#include <signal.h>
#include <pthread.h>  // 线程头文件
#endif // !WIN32



#ifndef TRACE
#define TRACE(str,...) {\
	char output[251]="";\
	snprintf(output, sizeof(output), "%s(%d)" str, __FILE__,__LINE__,##__VA_ARGS__);\
	OutputDebugStringA(output);}
#endif // !TRACE

#ifndef INVALID_HANDLE_VALUE
#ifndef WIN32
#define INVALID_HANDLE_VALUE 0
#endif // !WIN32
#endif // !INVALID_HANDLE_VALUE


/**
* 线程模板类
*/

// 要投递给线程的任务
class ThreadWorker
{
public:
	ThreadWorker()
		: m_function(NULL)
	{}
	/// <summary>
	/// 有参构造函数
	/// </summary>
	/// <typeparam name="_FUNCTION_">任务函数的类型</typeparam>
	/// <typeparam name="..._ARGS_">任务函数的参数</typeparam>
	/// <param name="func">任务函数指针</param>
	/// <param name="...args">任务函数的参数， 这里使用了可变参数模板</param>
	template<class _FUNCTION_, class... _ARGS_>
	ThreadWorker(_FUNCTION_ func, _ARGS_... args)
		: m_function(new CFunction<_FUNCTION_, _ARGS_...>(func, args...))
	{}
	/// <summary>
	/// 析构函数
	/// </summary>
	~ThreadWorker() {}
	/// <summary>
	/// 拷贝构造函数
	/// </summary>
	/// <param name="worker">类的实例化对象</param>
	ThreadWorker(const ThreadWorker& worker) {
		m_function = worker.m_function;
	}
	/// <summary>
	/// 等于号重载
	/// </summary>
	/// <param name="worker">类的实例化对象</param>
	/// <returns>返回值类型为ThreadWorker的引用</returns>
	ThreadWorker& operator=(const ThreadWorker& worker) {
		if (this != &worker) {
			m_function = worker.m_function;
		}
		return *this;
	}
	/// <summary>
	/// 重载括号运算符，仿函数所用
	/// </summary>
	/// <returns>返回值为任务函数的返回值</returns>
	int operator()() {
		if (m_function != NULL) {
			return (*m_function)();
		}
		return -1;
	}
	/// <summary>
	/// 任务是否有效
	/// </summary>
	/// <returns>返回值表示是否有效 true表示有效 false表示无效</returns>
	bool IsValid() const {
		return m_function != NULL;
	}
private:
	// 线程任务函数
	CFunctionBase* m_function;
};


class CThread
{
public:
	// 线程不允许复制构造和等于号重载
	CThread(const CThread&) = delete;
	CThread& operator=(const CThread&) = delete;
public:
	/// <summary>
	/// 无参构造
	/// </summary>
	CThread() {
#ifdef WIN32
		m_hThread = INVALID_HANDLE_VALUE;
#else
		m_thread = INVALID_HANDLE_VALUE;
#endif // WIN32
		m_iStatus = 0;
		m_worker.store(NULL);
	}
	/// <summary>
	/// 析构函数
	/// </summary>
	~CThread() {
		Stop();
	}
	/// <summary>
	/// 启动线程
	/// </summary>
	/// <returns>返回值表示线程启动是否成功 true表示成功 false表示失败</returns>
	bool Start() {
#ifdef WIN32
		m_iStatus = 1;
		m_hThread = (HANDLE)_beginthread(&CThread::ThreadEntry, 0, this);
		if (!IsValid()) {
			m_iStatus = 0;
		}
		return m_iStatus == 1;
#else
		m_iStatus = 1;
		do {
			pthread_attr_t attr;
			int ret = 0;
			ret = pthread_attr_init(&attr);
			if (ret != 0) {
				m_iStatus = 0;
				break;
			}
			ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); // PTHREAD_CREATE_DETACHED当这个线程结束了（调用这个结束）就完了，不需要主线程再去join它了
			if (ret != 0) {
				m_iStatus = 0;
				break;
			}
			// 创建线程
			ret = pthread_create(&m_thread, &attr, &CThread::ThreadEntry, this);
			if (ret != 0) {
				m_thread = INVALID_HANDLE_VALUE;
				m_iStatus = 0;
				break;
			}
			ret = pthread_attr_destroy(&attr);
			if (ret != 0) {
				m_thread = INVALID_HANDLE_VALUE;
				m_iStatus = 0;
				break;
			}
		} while (false);
		return m_iStatus == 1;
#endif // WIN32
	}
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool Stop() {
#ifdef WIN32
		if (m_iStatus == 0)return true;
		m_iStatus = 0;
		DWORD ret = WaitForSingleObject(m_hThread, 1000);
		if (ret == WAIT_TIMEOUT) {
			TerminateThread(m_hThread, -1);
		}
		UpdateWorker();
		return ret == WAIT_OBJECT_0;
#else
		if (m_iStatus == 0)return true;
		m_iStatus = 0;
		if (m_thread != INVALID_HANDLE_VALUE) {
			pthread_t thread = m_thread;
			m_thread = INVALID_HANDLE_VALUE;
			timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = 100 * 1000000;
			int ret = pthread_timedjoin_np(thread, NULL, &ts);
			if (ret == ETIMEDOUT) {
				// 等待超时
				pthread_detach(thread);
				pthread_kill(thread, SIGUSR1);
			}
		}
		UpdateWorker();
		return true;
#endif // WIN32
	}
	/// <summary>
	/// 是否有效
	/// </summary>
	/// <returns>返回值表示是否有效 true表示有效 false表示无效</returns>
	bool IsValid() {
#ifdef WIN32
		if ((m_hThread == NULL) || (m_hThread == INVALID_HANDLE_VALUE))return false;
		return WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT;
#else
		if (m_thread == INVALID_HANDLE_VALUE)return false;
		return ESRCH != pthread_kill(m_thread, 0); // ESRCH 表示线程不存在了
#endif // WIN32
	}
	/// <summary>
	/// 更新线程任务
	/// </summary>
	/// <param name="worker"></param>
	void UpdateWorker(const ThreadWorker& worker = ThreadWorker()) {
		if ((m_worker.load() != NULL) && (m_worker.load() != &worker/*代表是新的worker不是之前的，就需要把之前的释放掉*/)) {
			::ThreadWorker* pWorker = m_worker.load();
			m_worker.store(NULL);
			delete pWorker;
		}
		if (m_worker.load() == &worker) // 如果投递的任务指针一样，则直接返回
		{
			return;
		}
		if (!worker.IsValid()) {
			m_worker.store(NULL);
			return;
		}
		ThreadWorker* pWorker = new ThreadWorker(worker);
		if (pWorker == NULL) {

			printf("%s(%d)<%ld>:线程结束运行\r\n", __FILE__, __LINE__, m_thread);
			// TRACE("内存分配失败\r\n");
			return;
		}
		m_worker.store(pWorker);
	}
	/// <summary>
	/// 当前线程是否处于空闲状态
	/// </summary>
	/// <returns>返回值表示是否空闲 true表示空闲 false表示已经分配了任务</returns>
	bool IsIdle() {
		if (m_worker.load() == NULL)return true;
		// 任务不为空，下面判断任务是否是无效
		return false == m_worker.load()->IsValid();
	}
private:
	/// <summary>
	/// 线程的入口函数
	/// </summary>
	/// <param name="arg">传给线程的参数</param>
#ifdef WIN32
	static void ThreadEntry(void* arg) {
		CThread* thiz = (CThread*)arg;
		if (thiz) {
			thiz->ThreadMain();
		}
		_endthread();
	}
#else
	static void* ThreadEntry(void* arg) {
		CThread* thiz = (CThread*)arg;
		if (thiz == NULL)return NULL;
		struct sigaction act = { 0 };
		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_SIGINFO;
		act.sa_sigaction = &CThread::Sigaction;
		sigaction(SIGUSR1, &act, NULL); // 信号注册
		sigaction(SIGUSR2, &act, NULL);
		thiz->ThreadMain();
		// 线程结束处理
		pthread_t thread = pthread_self();
		pthread_detach(thread);
		pthread_exit(NULL);
		return NULL;
	}
	/// <summary>
	/// 处理线程信号
	/// </summary>
	/// <param name="signo">信号编号，区分不同的触发信号</param>
	/// <param name="info"></param>
	/// <param name="context"></param>
	static void Sigaction(int signo, siginfo_t* info, void* context) {
		if (signo == SIGUSR1) {
			// 线程退出信号
			pthread_exit(NULL);
		}
	}
#endif // WIN32


	/// <summary>
	/// 线程主函数
	/// </summary>
	void ThreadMain() {
		while (m_iStatus == 1) {
			if (m_worker.load() == NULL) {
				// 当前没有任务需要执行
#ifdef WIN32
				Sleep(1);
#else
				usleep(1000000);  // 1S
#endif // WIN32
				continue;
			}
			ThreadWorker worker = *m_worker.load();
			// printf("%s(%d)<%ld>:接收到任务\r\n", __FILE__, __LINE__, m_thread);
			if (worker.IsValid()) {
#ifdef WIN32
				if (WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT) {
#else

				if ((m_thread != INVALID_HANDLE_VALUE) && (ESRCH != pthread_kill(m_thread, 0))) { //ESRCH 3 ESRCH表示线程不存在了
#endif // WIN32
					int ret = worker(); // 执行任务
					printf("%s(%d)<%ld>:ret %d\r\n", __FILE__, __LINE__, m_thread, ret);
					// TODO:任务执行完对返回值的处理
					UpdateWorker();
				}
				else {
					// 线程结束
					printf("%s(%d)<%ld>:线程结束运行\r\n", __FILE__, __LINE__, m_thread);
					break;
				}
			}
			else {
#ifdef WIN32
				Sleep(1);
#else
				usleep(1000000);  // 1S
#endif // WIN32
			}
		}
	}
public:
#ifdef WIN32
	HANDLE                     m_hThread;  // 线程句柄
#else
	pthread_t                  m_thread;   // 线程id
#endif // WIN32
	int                        m_iStatus;  // 线程状态, 0表示线程将要关闭  1表示线程正在运行  2表示线程处于暂停状态
	std::atomic<ThreadWorker*> m_worker;   // 线程的任务  ****原子类也需要初始化，切记****
};

