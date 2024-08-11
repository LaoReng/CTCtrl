#pragma once
#include <iostream>
#include <cstdio>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <fcntl.h>
#include "CTCtrlThreadPool.h"
#include "CTCtrlEpoll.h"
#include "Packet.h"
#include "Process.h"

#include "CrossTimeServer.h"


// 对应着一个数据结构（结构体）
// 连接到文件服务器上的客户端信息保存结构类型
typedef struct FileClientInfo
{
public:
	// 

	FileClientInfo(int sock, sockaddr_in& addr)
		: m_sock(sock)
		, m_index(0)
	{
		memcpy(&m_addr, &addr, sizeof(m_addr));
		m_buf.resize(102400);
	}

	struct sockaddr_in  m_addr;   // 客户端地址信息
	int                 m_sock;   // 客户端套接字
	std::string         m_buf;    // 客户端对应的缓冲区
	std::string m_imageName;      // 图片的名字
	ssize_t             m_index;  // 缓冲区的索引位置
	std::mutex          m_lock;   // 缓冲区读写保护锁
}FCLIENTINFO;



class CFilesServer
{
public:
	/// <summary>
	/// 文件服务器进程主线程函数
	/// </summary>
	/// <param name="proc">参数</param>
	/// <returns></returns>
	static int CreateFileServer(CProcess* proc) {
		LOG_INFO << "files server start";
		CCrossTimeServer::m_logFile->flush();


		CFilesServer fs;
		int ret = fs.start();
		if (ret < 0) {
			LOG_ERROR << "文件服务器开启失败";
			CCrossTimeServer::m_logFile->flush();
			return -1;
		}
		int fd = 1;
		while (1) {
			proc->RecvFD(fd);
			if (fd <= 0) {
				LOG_INFO << "接收到结束文件描述符  fd:" << fd;
				CCrossTimeServer::m_logFile->flush();
				break;
			}
		}

		LOG_INFO << "files server end";
		CCrossTimeServer::m_logFile->flush();

		return 0;
	}
public:
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="TPool_cnt">线程池中的线程数量</param>
	/// <param name="sock_lis_cnt">套接字监听时设置的数量</param>
	/// <param name="sock_port">服务器端口</param>
	/// <param name="sock_bind_ip">服务器IP地址</param>
	CFilesServer(
		int TPool_cnt = 4,
		int sock_lis_cnt = 5,
		short sock_port = FILESERVER_PORT,
		const std::string& sock_bind_ip = SERVER_IP
	);
	/// <summary>
	/// 析构函数
	/// </summary>
	~CFilesServer();
	/// <summary>
	/// 开启服务器
	/// </summary>
	/// <returns>返回值表示服务器是否开启成功 返回0表示成功  -1表示失败</returns>
	int start();
private:
	typedef int(CFilesServer::* DIS_FUNC)(const UrlParser&,
		const CHttpParser&, std::string&,
		std::string&,
		SOCKET);                                                     // 成员函数的函数指针


	/// <summary>
	/// 服务器套接字初始化
	/// </summary>
	/// <returns>返回值表示初始化是否成功 0表示成功 -1表示失败</returns>
	int sockInit();
	/// <summary>
	/// 关闭套接字
	/// </summary>
	void socketClose();
	/// <summary>
	/// 初始化函数映射表
	/// </summary>
	void initFuncMapping();
	/// <summary>
	/// 任务投递
	/// </summary>
	/// <returns>返回值为0 表示任务投递成功</returns>
	int Dispose();
	/// <summary>
	/// 线程任务函数
	/// </summary>
	/// <param name="argc">传递给线程函数的参数</param>
	/// <returns></returns>
	static int MyWorker(void* argc);
	/// <summary>
	/// 线程任务函数主函数
	/// </summary>
	void mainWorker();
	/// <summary>
	/// 设置套接字为非阻塞模式
	/// </summary>
	/// <param name="fd">要设置非阻塞的文件描述符</param>
	/// <returns>返回值表示是否设置成功  返回0则表示设置成功  返回值为-1则表示设置失败</returns>
	int setNoblock(int fd);
	/// <summary>
	/// HTTP协议格式数据解析
	/// </summary>
	int HttpParser(const CHttpParser& parser, std::string& header, std::string& bodyData, SOCKET clientSocket);
	/// <summary>
	/// 制作http应答包
	/// </summary>
	/// <param name="bodyData">要返回给用户的响应体</param>
	/// <returns>返回http应答的完整包</returns>
	Buffer MakeAckPacket(std::string& header, std::string& bodyData);
private:
	// 业务处理函数
	// 获取指定名字文件的业务处理
	int getImageDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& header,
		std::string& bodyData,
		SOCKET clientSocket = -1);
private:
	CThreadPool                  m_ThreadPool;    // 线程池
	CEpoll                       m_ep;            // epoll
	int                          m_serverSock;    // 服务器套接字
	int                          m_listenCnt;     // 套接字监听数
	short                        m_serverPort;    // 套接字端口
	std::string                  m_serverIp;      // 服务器IP地址
	std::map<int, FCLIENTINFO*>  m_mapClient;     // 每个连接上来的客户端对应一个缓冲区buf
	std::map<std::string, DIS_FUNC>         m_mGETUriDisfunc;        // GET请求函数映射表  uri(GET)->dispose
	std::map<std::string, DIS_FUNC>         m_mPOSTUriDisfunc;       // POST请求函数映射表 uri(POST)->dispose
	std::map<std::string, DIS_FUNC>         m_mDELETEUriDisfunc;     // DELETE请求函数映射表 uri(DELETE)->dispose
	std::map<std::string, std::string> m_mImageData; // 图片数据映射表
	std::mutex          m_ImageDatalock;   // 缓冲区读写保护锁
};

