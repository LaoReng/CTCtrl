#pragma once

#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>      // 文件操作头文件
#include <unistd.h>
#include <vector>
#include <list>
#include <string.h>     // memset头文件
#include <sys/select.h> // select头文件
#include <sys/socket.h> // SOCKET相关头文件
#include "DBHelper.h"   // 数据库操作头文件
#include "Public.h"     // 共有资源类
#include "HttpParser.h" // http解析封装类
#include "Crypto.h"     // 加解密协议
#include "jsoncpp/json.h" // json结构分装和解析

using namespace muduo;

typedef struct SupEquipInfo
{
	SupEquipInfo()
		: recvImageSock(INVALID_SOCKET)
		, recvImagePort(0)
		, sendImageClientSock(INVALID_SOCKET)
	{}
	SupEquipInfo(const SupEquipInfo& se)
		: recvImageSock(se.recvImageSock)
		, recvImagePort(se.recvImagePort)
		, ctrlerId(se.ctrlerId.c_str())
		, imageDir(se.imageDir.c_str())
		, sendImageClientSock(se.sendImageClientSock)
	{}
	SupEquipInfo& operator=(const SupEquipInfo& se) {
		if (this != &se) {
			recvImageSock = se.recvImageSock;
			recvImagePort = se.recvImagePort;
			ctrlerId = se.ctrlerId.c_str();
			imageDir = se.imageDir.c_str();
			sendImageClientSock = se.sendImageClientSock;
		}
		return *this;
	}
	~SupEquipInfo() {}
	SOCKET         recvImageSock;       // 接收图片的套接字
	unsigned short recvImagePort;       // 接收图片的端口
	std::string    ctrlerId;            // 控制者的id
	std::string    imageDir;            // 图片存放的绝对位置
	SOCKET         sendImageClientSock; // 发送图片的被控端套接字
	// TODO:...
} SUPEQUIPINFO;

class CCrossTimeServer
{
public:
	CCrossTimeServer(const std::string& ip = "0.0.0.0", unsigned short port = 9668);
	~CCrossTimeServer();
	// 开启服务器，开启select服务
	bool Start();
	// 停止服务器运行
	void Stop();
private:
	/**
	* @brief 对客户端发送过来的数据解析和处理
	* @param clientSocket 有事件发生的客户端套接字
	* @return
	*	-1:表示客户端主动断开连接;
	*	 0:表示没有问题处理完成;
	*/
	int Dispose(SOCKET clientSocket);
	// 连接客户端
	bool Accept();
	// Select服务器初始化，socket初始化
	bool SelectInit();
	// 清空所有数据信息
	void Clear();
	// 设置select的监听套接字，通过m_fds进行辅助设置
	bool SetRdSet();
	// HTTP协议解析
	int HttpParser(const Buffer& data, std::string& bodyData, SOCKET clientSocket = 0);
	// 数据库初始化
	bool initDatabase();

private: // 业务处理
	// 注册业务处理
	int registerDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 登录业务处理
	int loginDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 被控端上线业务处理
	int bectrlOnlineDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 获取设备列表业务处理
	int equipmentlistDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 开始设备监控业务处理
	int startEquipmentCtrlDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 停止设备监控业务处理
	int endEquipmentCtrlDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 鼠标键盘业务处理
	int ctrlEventDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 忘记密码业务处理
	int forgetPasswordDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 答案验证业务处理
	int verifyAnswerDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 获取个人信息业务处理
	int getUserInfoDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 修改用户信息业务处理
	int updateUserInfoDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	int getNewNoticeDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	int getAllNoticeDispose(const UrlParser& url, const CHttpParser& parser, std::string& bodyData, SOCKET clientSocket = -1);
	// 被控端图片发送线程入口函数
	static void* ScreenMonitorThreadEnt(void* arg);
	// 被控端图片发送线程主函数
	int ScreenMonitorThreadMain(SOCKET beSock, const std::string& eName);
	// 制作http应答包
	Buffer MakeAckPacket(std::string& bodyData);
	// 寻找文件描述符最大值
	SOCKET maxFind();
	// 初始化日志服务器
	void initLogger();
	// 日志输出函数
	static void outputFunc(const char* msg, int len);
	// 日志刷新函数
	static void flushFunc();
private:
	SOCKET                                  m_serverSocket;          // 服务器socket，listenSocket
	sockaddr_in                             m_serverAddr;            // 服务器地址绑定信息
	std::list<SOCKET>                       m_fds;                   // 保存连接上来的客户端，包括服务器套接字
	std::map<SOCKET, clientInfo>            m_mapSockAddr;           // 套接字与地址信息映射表
	int                                     m_maxFd;                 // 最大的文件描述符值，select所用
	fd_set                                  m_rdset;                 // 读事件，用于select监听
	bool                                    m_bServerInvalid;        // 服务器是否有效 true表示有效 false表示无效
	DBHelper                                m_db;                    // 数据库
	std::map<std::string, SUPEQUIPINFO>     m_mbEqu;                 // 当前控制设备的信息 【当前】设备名 -> {设备信息}
public:
	static std::unique_ptr<LogFile>         m_logFile;               // 服务器日志库
};

