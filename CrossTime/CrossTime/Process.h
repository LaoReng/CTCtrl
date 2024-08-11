#pragma once

#include "CTCtrlFunction.h"
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <Process.h>
#include <unistd.h>

// 进程控制类
class CProcess
{
public:
	/**
	* @brief 构造函数
	*/
	CProcess() {
		m_func = NULL;
		memset(m_pipes, 0, sizeof(m_pipes));
	}
	/**
	* @brief 析构函数
	*/
	~CProcess() {
		if (m_func != NULL) {
			delete m_func;
			m_func = NULL;
		}
	}
	/**
	* @brief 设置进程入口函数
	* @param func  入口函数指针
	* @param args  入口函数的参数，这里用的是可变参数模板，因为每个进程参数的个数是不固定的
	*
	* @return 返回值为0，则表示进程入口函数设置成功；为非0，则表示入口函数设置失败
	*/
	template<typename _FUNCTION_, typename... _ARGS_>
	int SetEntryFunction(_FUNCTION_ func, _ARGS_... args) {
		m_func = new CFunction<_FUNCTION_, _ARGS_...>(func, args...);
		if (m_func == NULL) {
			return -1;
		}
		return 0;
	}
	/**
	* @brief 创建管道用于进程间通信，创建进程
	*
	* @return 返回值为0，表示创建成功；为非0，则表示创建失败
	*/
	int CreateSubProcess() {
		if (m_func == NULL)
			return -1;
		int ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, m_pipes);  // 本地套接字
		printf("%s(%d):<%s> pipe[0]=%d, pipe[1]=%d\n", __FILE__, __LINE__, __FUNCTION__, m_pipes[0], m_pipes[1]);
		if (ret == -1)
			return -2;
		pid_t pid = fork();
		if (pid == -1)
			return -2;
		if (pid == 0) {
			// 子进程
			close(m_pipes[1]); // 关闭掉写
			m_pipes[1] = 0;
			ret = (*m_func)();
			exit(0);
		}
		// 主进程
		close(m_pipes[0]);
		m_pipes[0] = 0;
		m_pid = pid;
		return 0;
	}
	/**
	* @brief 通过管道向进程发送文件描述符函数
	* @param fd 要发送的文件描述符
	*
	* @return 返回值为0，则表示发送成功；为非0，则表示发送失败
	*/
	int SendFD(int fd) { // 主进程
		struct msghdr msg;
		memset(&msg, 0, sizeof(msg));
		iovec iov[2];
		char buf[2][10] = { "1111","2222" };
		iov[0].iov_base = buf[0];
		iov[0].iov_len = sizeof(buf[0]);
		iov[1].iov_base = buf[1];
		iov[1].iov_len = sizeof(buf[1]);
		msg.msg_iov = iov;
		msg.msg_iovlen = 2;
		// 下面的数据，才是我们需要传递的
		cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
		if (cmsg == NULL)return -1;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET; // 级别是套接字的连接
		cmsg->cmsg_type = SCM_RIGHTS;
		*((int*)CMSG_DATA(cmsg)) = fd;
		msg.msg_control = cmsg;
		msg.msg_controllen = cmsg->cmsg_len;
		ssize_t ret = sendmsg(m_pipes[1], &msg, 0);
		free(cmsg);
		if (ret == -1) {
			printf("%s(%d):<%s> ret=%d\n", __FILE__, __LINE__, __FUNCTION__, ret);
			return -2;
		}
		return 0;
	}
	/**
	* @brief 通过管道接收文件描述符函数
	* @param fd 保存接收到的文件描述符
	*
	* @return 返回值为0，则表示接收成功；为非0，则表示接收失败
	*/
	int RecvFD(int& fd) {
		struct msghdr msg;
		memset(&msg, 0, sizeof(msg));
		iovec iov[2];
		char buf[][10] = { "","" };
		iov[0].iov_base = buf[0];
		iov[0].iov_len = sizeof(buf[0]);
		iov[1].iov_base = buf[1];
		iov[1].iov_len = sizeof(buf[1]);
		msg.msg_iov = iov;
		msg.msg_iovlen = 2;

		cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
		if (cmsg == NULL)return -1;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		msg.msg_control = cmsg;
		msg.msg_controllen = CMSG_LEN(sizeof(int));
		ssize_t ret = recvmsg(m_pipes[0], &msg, 0);
		if (ret == -1) {
			free(cmsg);
			return -2;
		}
		fd = *(int*)CMSG_DATA(cmsg);
		free(cmsg);
		return 0;
	}
	/**
	* @brief 通过管道向进程发送网络套接字和地址信息
	* @param fd   要发送的套接字文件描述符
	* @param addrin  要发送的文件描述符地址信息
	*
	* @return 返回值为0，则表示发送成功；为非0，则表示发送失败
	*/
	int SendSocket(int fd, const sockaddr_in* addrin) { // 主进程
		struct msghdr msg;
		memset(&msg, 0, sizeof(msg));
		iovec iov;
		char buf[20] = "";
		memcpy(buf, addrin, sizeof(sockaddr_in));
		iov.iov_base = buf;
		iov.iov_len = sizeof(buf);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		// 下面的数据，才是我们需要传递的
		cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
		if (cmsg == NULL)return -1;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET; // 级别是套接字的连接
		cmsg->cmsg_type = SCM_RIGHTS;
		*((int*)CMSG_DATA(cmsg)) = fd;
		msg.msg_control = cmsg;
		msg.msg_controllen = cmsg->cmsg_len;
		ssize_t ret = sendmsg(m_pipes[1], &msg, 0);
		free(cmsg);
		if (ret == -1) {
			printf("%s(%d):<%s> ret=%d\n", __FILE__, __LINE__, __FUNCTION__, ret);
			return -2;
		}
		return 0;
	}
	/**
	* @brief 通过管道接收网络套接字和地址信息
	* @param fd  保存接收到的套接字文件描述符
	* @param addrin  保存接收到的套接字描述符地址信息
	*
	* @return 返回值为0，则表示接收成功；为非0，则表示接收失败
	*/
	int RecvSocket(int& fd, sockaddr_in* addrin) {
		struct msghdr msg;
		memset(&msg, 0, sizeof(msg));
		iovec iov;
		char buf[20] = "";
		iov.iov_base = buf;
		iov.iov_len = sizeof(buf);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;

		cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
		if (cmsg == NULL)return -1;
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		msg.msg_control = cmsg;
		msg.msg_controllen = CMSG_LEN(sizeof(int));
		ssize_t ret = recvmsg(m_pipes[0], &msg, 0);
		if (ret == -1) {
			free(cmsg);
			return -2;
		}
		fd = *(int*)CMSG_DATA(cmsg);
		memcpy(addrin, buf, sizeof(sockaddr_in));
		free(cmsg);
		return 0;
	}
	/**
	* @brief 开启守护进程
	*
	* @return 返回值为0，则表示守护进程创建成功；为非0，则表示守护进程创建失败
	*/
	static int SwitchDeamon() {
		pid_t ret = fork();
		if (ret == -1)return -1;
		if (ret > 0) {
			// 主进程到此为止
			exit(0);
		}
		// 子进程内容如下
		ret = setsid();
		if (ret == -1)return -2;
		ret = fork();
		if (ret == -1)return -3;
		if (ret > 0) {
			exit(0);  // 子进程到此为止
		}

		// 孙进程的内容如下，进入守护状态
		for (int i = 0; i < 3; i++) {
			close(i); // 关闭输入输出，这是为了防止开启守护进程后，在有数据打印到命令行上
		}
		umask(0);
		signal(SIGCHLD, SIG_IGN);
		return 0;
	}
private:
	// 存进程函数
	CFunctionBase* m_func;
	// 存进程id
	pid_t m_pid;
	// 进程间通信管道
	int m_pipes[2];
};


