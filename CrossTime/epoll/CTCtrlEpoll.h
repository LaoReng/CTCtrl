#pragma once
#include <cstdio>
#include <unistd.h>
#include <sys/epoll.h>

// TODO:对EPOLL进行操作的时候，是不是需要加锁呢？？
class CEpoll
{
public:
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="size">创建epoll所传递的size，伪值</param>
	CEpoll(int size = 1);
	/// <summary>
	/// 析构函数
	/// </summary>
	~CEpoll();
	/// <summary>
	/// 初始化【可以不调用】
	/// </summary>
	/// <param name="size">创建epoll所传递的size，伪值</param>
	/// <returns>初始化成功，则返回值为0；初始化失败，返回值为-1
	/// errno查看错误码   strerror查看错误信息
	/// </returns>
	int Init(int size = 1);
	/// <summary>
	/// 向epoll添加文件描述符
	/// </summary>
	/// <param name="fd">要添加到epoll的文件描述符</param>
	/// <param name="ev">该文件描述符上要监听的事件和携带的数据</param>
	/// <returns>添加成功，则返回值为0；添加失败，返回值为-1
	/// errno查看错误码   strerror查看错误信息
	/// </returns>
	int Add_fd(int fd, struct epoll_event* ev);
	/// <summary>
	/// 修改epoll中存在的文件描述符
	/// </summary>
	/// <param name="fd">要修改到epoll的文件描述符</param>
	/// <param name="ev">该文件描述符上要监听的事件和携带的数据</param>
	/// <returns>修改成功，则返回值为0；修改失败，返回值为-1
	/// errno查看错误码   strerror查看错误信息
	/// </returns>
	int Mod_fd(int fd, struct epoll_event* ev);
	/// <summary>
	/// 将文件描述符从epoll删除
	/// </summary>
	/// <param name="fd">要删除的文件描述符</param>
	/// <returns>删除成功，则返回值为0；删除失败，返回值为-1
	/// errno查看错误码   strerror查看错误信息
	/// </returns>
	int Del_fd(int fd);
	/// <summary>
	/// 等待epoll中有事件触发
	/// </summary>
	/// <param name="evlist">epoll_event结构体数组，保存有关就绪态文件描述符的信息</param>
	/// <param name="Maxevents">指定evlist数组中可以包含的最大元素个数</param>
	/// <param name="timeout">设置等待阻塞行为，-1表示一直阻塞；0表示执行一次非阻塞地检查；大于0表示阻塞至多timeout毫秒，当有事件发生时会提前返回</param>
	/// <returns>返回值表示evlist数组中元素的个数 出现错误则返回值小于0  errno查看错误码   strerror查看错误信息</returns>
	int EWait(struct epoll_event* evlist, int Maxevents, int timeout);
	/// <summary>
	/// 关闭epoll套接字
	/// </summary>
	/// <returns></returns>
	int EClose();
private:
	int m_epfd;
};

