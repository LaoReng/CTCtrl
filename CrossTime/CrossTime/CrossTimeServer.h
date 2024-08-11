#pragma once

#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>        // 文件操作头文件
#include <unistd.h>
#include <vector>
#include <list>
#include <string.h>       // memset头文件
#include <sys/select.h>   // select头文件
#include <sys/socket.h>   // SOCKET相关头文件
#include "DBHelper.h"     // 数据库操作头文件
#include "Public.h"       // 共有资源类
#include "Tools.h"        // 工具类
#include "HttpParser.h"   // http解析封装类
#include "Crypto.h"       // 加解密协议
#include "jsoncpp/json.h" // json结构分装和解析
#include "CTCtrlThreadPool.h"
#include "Process.h"
#include "Packet.h"

using namespace muduo;

#define DB_USER          "root"        // 远程连接数据库用户名
#define DB_PASSWORD      "123456"   // 数据库用户密码  CTCtrl666  123
#define SERVER_IP        "0.0.0.0"     // 服务器IP地址
#define WEBSERVER_PORT   9668          // web服务器绑定端口
#define FILESERVER_PORT  9999          // 文件服务器绑定端口

#define BUFFER_SIZE 1024000


class CCrossTimeServer
{
public:
	CCrossTimeServer(const std::string& ip = SERVER_IP, unsigned short port = WEBSERVER_PORT);
	~CCrossTimeServer();
	// 开启服务器，开启select服务
	bool Start();
	// 停止服务器运行
	void Stop();
private:

#if 1
	// 被控端图片发送线程入口函数
	static void* ScreenMonitorThreadEnt(void* arg) {
		screenArgs* pArgs = (screenArgs*)arg;
		screenArgs args = *pArgs;
		delete pArgs;
		CCrossTimeServer* thiz = args.thiz;
		thiz->ScreenMonitorThreadMain(args.beSocket, args.eName);
		return nullptr;
	}
	// 被控端图片发送线程主函数
	int ScreenMonitorThreadMain(SOCKET beSock, const std::string& eName) {
		int count = 6; // 获取6次
		while (count--) {
			auto it = m_mbEqu.find(eName);
			if (it == m_mbEqu.end()) {
				// 没找到当前这个设备的信息
				return -1;
			}
			if (it->second.recvImageSock == INVALID_SOCKET) {
				// 还没有更新过来
				continue;
			}
			else {
				break;
			}
			sleep(5);
		}
		auto it = m_mbEqu.find(eName);
		if (it == m_mbEqu.end()) {
			return -1;
		}
		SOCKET sock = it->second.recvImageSock;
		if ((count <= 0) && (sock == INVALID_SOCKET)) {
			// 可能存在问题
			return -1;
		}
		std::string path = it->second.imageDir.c_str();
		unsigned short port = it->second.recvImagePort;

		// 通知被控端连接服务器
		char str[] = "开始控制";
		CPacket pack(1, port, str, sizeof(str));
		ssize_t len = send(beSock, pack.outStr(), pack.Size(), 0);
		if (len <= 0) {
			LOG_ERROR << "数据发送失败";
			m_logFile->flush();
			close(sock);
		}

		sockaddr_in clientAddr = {};
		int clientSock = INVALID_SOCKET;
		do {
			socklen_t AddrLen = sizeof(clientAddr);
			clientSock = accept(sock, (sockaddr*)&clientAddr, &AddrLen);
			if (clientSock < 0) {
				// 套接字连接失败
				LOG_ERROR << "套接字连接失败";
				m_logFile->flush();
				break;
			}
			// 被控端连接上来
			m_mbEqu.find(eName)->second.sendImageClientSock = clientSock;
			// 将客户端套接字设置为非阻塞模式
			int flags = 0;
			if ((flags = fcntl(clientSock, F_GETFL, 0)) < 0) {
				LOG_ERROR << "获取套接字配置选项失败";
				m_logFile->flush();
				break;
			}
			flags |= O_NONBLOCK;
			if (fcntl(clientSock, F_SETFL, flags) < 0) {
				LOG_ERROR << "设置套接字配置选项失败";
				m_logFile->flush();
				break;
			}

			std::string buf;
			buf.resize(BUFFER_SIZE);
			char* pData = (char*)buf.c_str();
			long long index = 0;
			int count = 0;
			while (true) {
				long long len = recv(clientSock, pData + index, BUFFER_SIZE - index, 0);
				if (len <= 0) {
					if (EAGAIN == errno) { // 没有错误，只是非阻塞
						if (index > 0) {
							// 证明里面还有数据
							len = 0;
						}
						else if (len == 0) {
							break;
						}
						else {
							usleep(500);
							continue;
						}
					}
					else {
						LOG_ERROR << "监控线程退出";
						m_logFile->flush();
						break;
					}
				}
				index += len;
				// TODO:处理命令
				len = index;
				if (len == 0) {
					count++;
					if (count >= 10)break;
					continue;
				}
				count = 0;
				CPacket m_packet(pData, len);
				if (len > 0) {
					memmove(pData, pData + len, BUFFER_SIZE - len);
					switch (m_packet.GetCmd()) {
					case 1: // 屏幕数据
						int fd = open(path.c_str(), O_RDWR | O_CREAT, 0666);
						if (fd < 0) {
							LOG_ERROR << "文件创建失败！[" << path.c_str() << "]";
							m_logFile->flush();
							goto BREAK;
						}
						ssize_t ret = write(fd, m_packet.GetData(), m_packet.GetDataSize());
						LOG_INFO << "图片写入字节数 ret=" << ret << " 图片位置:[" << path << "]";
						m_logFile->flush();
						if (ret < 0) {
							LOG_ERROR << "写入错误 ret = " << ret;
							m_logFile->flush();
						}
						close(fd);
						break;
					}
				}
				else if (len < 0) {
					// 数据包校验和失败
					len = -1 * len;
					memmove(pData, pData + len, BUFFER_SIZE - len);
				}
				index -= len;
			}
		} while (false);
	BREAK:
		// 设置端口复用
		int enable = 1;
		int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
		if (ret < 0) {
			LOG_INFO << "setsockopt ret=" << ret << "errno=" << errno;
			m_logFile->flush();
		}
		close(clientSock);
		close(sock);
		LOG_INFO << "图片接收线程结束";
		m_logFile->flush();
		return 0;
	}
#endif

	/// <summary>
	/// 对客户端发送过来的数据解析和处理
	/// </summary>
	/// <param name="clientSocket">有事件发生的客户端套接字</param>
	/// <returns>-1:表示客户端主动断开连接;  0:表示没有问题处理完成;</returns>
	int Dispose(SOCKET clientSocket);
	// 连接客户端
	bool Accept();
	// Select服务器初始化，socket初始化
	bool SelectInit();
	// 清空所有数据信息
	void Clear();
	// 设置select的监听套接字(读集合)，通过m_fds进行辅助设置
	bool SetRdSet();
	// HTTP协议解析
	int HttpParser(
		const Buffer& data,
		std::string& bodyData,
		SOCKET clientSocket = 0);
	// 数据库初始化
	bool initDatabase();

	void MyWorkerMain(const Buffer& buf, SOCKET clientSocket) {
		LOG_INFO << "任务函数被执行" << " buf:" << buf.c_str() << "socket:" << clientSocket;
		m_logFile->flush();

		std::string bodyData; // 应答包中的body数据部分
		int ret = HttpParser(buf, bodyData, clientSocket);
		if (ret < 0) {
			LOG_WARN << "http数据解析失败";
			m_logFile->flush();
		}
		Buffer response = MakeAckPacket(bodyData);
		LOG_INFO << "response body[" << bodyData << "]";
		m_logFile->flush();
		ssize_t len = send(
			clientSocket,
			response.c_str(),
			response.size(),
			0);
		if (len <= 0) {
			// return -1;
			return;
		}
		// return 0;
		return;
	}
	// 任务函数
	static int MyWorkerEny(
		void* arg,
		const Buffer buf,
		SOCKET clientSocket) {
		if (arg == NULL)return -1;
		CCrossTimeServer* thiz = (CCrossTimeServer*)arg;
		thiz->MyWorkerMain(buf, clientSocket);
		return 0;
	}
private: // 业务处理
	// 测试请求
	int testDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 注册业务处理
	int registerDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 登录业务处理
	int loginDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 被控端上线业务处理
	int bectrlOnlineDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 被控端下线业务处理
	int bectrlDownlineDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取头像列表业务处理
	int getAvatarListDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取设备列表业务处理
	int equipmentlistDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 开始设备监控业务处理
	int startEquipmentCtrlDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 停止设备监控业务处理
	int endEquipmentCtrlDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 设备关机|重启|删除业务处理
	int controlEquipDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取网站信息汇总
	int dataSummaryDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取当前系统在线人数
	int currentOnlineDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取指定位置的天气信息业务处理
	int getWeatherDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取所有注册设备
	int getEquipAllDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 鼠标键盘业务处理
	int ctrlEventDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 忘记密码业务处理
	int forgetPasswordDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 答案验证业务处理
	int verifyAnswerDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取个人信息业务处理
	int getUserInfoDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取控制端到被控端
	int ctrlToBectrlDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 修改用户信息业务处理
	int updateUserInfoDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取最新的通知公告
	int getNewNoticeDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 获取所有的通知公告
	int getAllNoticeDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);

	/// 后台
	// 后台登录业务处理
	int backLoginDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台获取用户列表业务处理
	int backGetUserListDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台根据地址筛选用户信息业务处理
	int backSearchUserListDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 用户删除、禁用、启用业务处理
	int backOperateUserDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台删除用户信息业务处理
	int backDeleteUserDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台删除设备业务处理
	int backDeleteDevicesDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台获取已同意的设备列表业务处理
	int backGetDevicesListDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台根据用户id筛选已同意的设备信息业务处理
	int backSearchDevicesListDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台获取未处理的申请设备业务处理
	int backGetApplyDevicesDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台处理设备申请业务处理
	int backApplyDevicesDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台通知发布业务处理
	int backIssueNoticeDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台获取通知列表业务处理
	int backGetNoticeListDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台通知更改业务处理
	int backUpdateNoticeDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 后台通知删除业务处理
	int backDeleteNoticeDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& bodyData,
		SOCKET clientSocket = -1);
	// 制作http应答包
	Buffer MakeAckPacket(std::string& bodyData);
	// 寻找文件描述符最大值
	SOCKET maxFind();
	// 初始化日志服务器
	void initLogger();
	// 初始化函数映射表
	void initFuncMapping();
	// 日志输出函数
	static void outputFunc(const char* msg, int len);
	// 日志刷新函数
	static void flushFunc();
	// 根据设备名称查找在线设备 返回值为在线设备在内存中的key
	std::map<SOCKET, clientInfo>::key_type findEInfo(std::string Ename);
private:
	typedef int(CCrossTimeServer::* DIS_FUNC)(const UrlParser&,
		const CHttpParser&,
		std::string&,
		SOCKET);                                                     // 成员函数的函数指针
	SOCKET                                  m_serverSocket;          // 服务器socket，listenSocket
	sockaddr_in                             m_serverAddr;            // 服务器地址绑定信息
	std::list<SOCKET>                       m_fds;                   // 保存连接上来的客户端，包括服务器套接字
	std::map<SOCKET, clientInfo>            m_mapSockAddr;           // 套接字与地址信息映射表
	int                                     m_maxFd;                 // 最大的文件描述符值，select所用
	fd_set                                  m_rdset;                 // 读事件，用于select监听
	bool                                    m_bServerInvalid;        // 服务器是否有效 true表示有效 false表示无效
	DBHelper                                m_db;                    // 数据库
	std::map<std::string, SUPEQUIPINFO>     m_mbEqu;                 // 当前控制设备的信息 【当前】设备名 -> {设备信息}
	//TODO:有问题暂不使用 CThreadPool                             m_ThreadPool;            // 执行任务的线程池
	std::map<std::string, DIS_FUNC>         m_mGETUriDisfunc;        // GET请求函数映射表  uri(GET)->dispose
	std::map<std::string, DIS_FUNC>         m_mPOSTUriDisfunc;       // POST请求函数映射表 uri(POST)->dispose
	std::map<std::string, DIS_FUNC>         m_mDELETEUriDisfunc;     // DELETE请求函数映射表 uri(DELETE)->dispose
public:
	static std::unique_ptr<LogFile>         m_logFile;               // 服务器日志库
};

