#pragma once
#include "Packet.h"
#include <atomic>   // ԭ�Ӳ���
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
	// windows���绷����ʼ��
	bool InitSocketEnv() {
		WSADATA data = {};
		if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
			MessageBox(NULL, _T("���������Ƿ�����"), _T("����"), MB_OK | MB_ICONERROR);
			return false;
		}
		return true;
	}
	// ���ӷ�����
	int connect(const sockaddr_in& addr) {
		if (m_sock == INVALID_SOCKET)return -1;
		return ::connect(m_sock, (sockaddr*)&addr, sizeof(addr));
	}
	// �����׽���recvΪ������ģʽ
	int setUnlock() {
		if (m_sock == INVALID_SOCKET)return -1;
		u_long mode = 1;
		return ::ioctlsocket(m_sock, FIONBIO, &mode);
	}
	// ��������
	int send(const std::string& data) {
		if (m_sock == INVALID_SOCKET)return -1;
		return ::send(m_sock, data.c_str(), (int)data.size(), 0);
	}
	// ��������
	int send(const char* pData, size_t size) {
		if ((pData == NULL) || (size <= 0) || (m_sock == INVALID_SOCKET))return -1;
		return ::send(m_sock, pData, (int)size, 0);
	}
	// ��������
	int recv(std::string& buffer) {
		if (m_sock == INVALID_SOCKET)return -1;
		return ::recv(m_sock, (char*)buffer.c_str(), (int)buffer.size(), 0);
	}
	// ��������
	int recv(char* pData, int len, int flags) {
		if (m_sock == INVALID_SOCKET)return -1;
		return ::recv(m_sock, pData, len, flags);
	}
	// �ر��׽���
	void Close() {
		if (m_sock != INVALID_SOCKET) {
			SOCKET temp = m_sock;
			m_sock = INVALID_SOCKET;
			closesocket(temp);
		}
	}
private:
	int                 m_type;      // Э������
	int                 m_nProtocol; // ָ��Э����ض�ֵ
	std::atomic<SOCKET> m_sock;      // ����ͻ����׽���
};

