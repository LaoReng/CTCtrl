#pragma once
#include "Packet.h"
#include <atomic>   // 原子操作
class CSocket
{
public:
	CSocket(int type = SOCK_STREAM, int nProtocol = 0)
		: m_type(type)
		, m_nProtocol(nProtocol)
	{
		InitSocketEnv();
		m_sock = socket(AF_INET, m_type, m_nProtocol);
	}
	~CSocket() {
		Close();
		WSACleanup();
	}
	// windows网络环境初始化
	bool InitSocketEnv() {
		WSADATA data = {};
		if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
			MessageBox(NULL, _T("请检查网络是否连接"), _T("错误"), MB_OK | MB_ICONERROR);
			return false;
		}
		return true;
	}
	// 连接服务器
	int connect(const sockaddr_in& addr) {
		if (m_sock == INVALID_SOCKET)return -1;
		return ::connect(m_sock, (sockaddr*)&addr, sizeof(addr));
	}
	// 设置套接字recv为非阻塞模式
	int setUnlock() {
		if (m_sock == INVALID_SOCKET)return -1;
		u_long mode = 1;
		return ::ioctlsocket(m_sock, FIONBIO, &mode);
	}
	// 发送数据
	int send(const std::string& data) {
		if (m_sock == INVALID_SOCKET)return -1;
		return ::send(m_sock, data.c_str(), (int)data.size(), 0);
	}
	// 发送数据
	int send(const char* pData, size_t size) {
		if ((pData == NULL) || (size <= 0) || (m_sock == INVALID_SOCKET))return -1;
		return ::send(m_sock, pData, (int)size, 0);
	}
	// 接收数据
	int recv(std::string& buffer) {
		if (m_sock == INVALID_SOCKET)return -1;
		return ::recv(m_sock, (char*)buffer.c_str(), (int)buffer.size(), 0);
	}
	// 接收数据
	int recv(char* pData, int len, int flags) {
		if (m_sock == INVALID_SOCKET)return -1;
		return ::recv(m_sock, pData, len, flags);
	}
	// 关闭套接字
	void Close() {
		if (m_sock != INVALID_SOCKET) {
			SOCKET temp = m_sock;
			m_sock = INVALID_SOCKET;
			closesocket(temp);
		}
	}
private:
	int                 m_type;      // 协议类型
	int                 m_nProtocol; // 指定协议的特定值
	std::atomic<SOCKET> m_sock;      // 保存客户端套接字
};

