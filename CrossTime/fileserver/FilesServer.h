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


// ��Ӧ��һ�����ݽṹ���ṹ�壩
// ���ӵ��ļ��������ϵĿͻ�����Ϣ����ṹ����
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

	struct sockaddr_in  m_addr;   // �ͻ��˵�ַ��Ϣ
	int                 m_sock;   // �ͻ����׽���
	std::string         m_buf;    // �ͻ��˶�Ӧ�Ļ�����
	std::string m_imageName;      // ͼƬ������
	ssize_t             m_index;  // ������������λ��
	std::mutex          m_lock;   // ��������д������
}FCLIENTINFO;



class CFilesServer
{
public:
	/// <summary>
	/// �ļ��������������̺߳���
	/// </summary>
	/// <param name="proc">����</param>
	/// <returns></returns>
	static int CreateFileServer(CProcess* proc) {
		LOG_INFO << "files server start";
		CCrossTimeServer::m_logFile->flush();


		CFilesServer fs;
		int ret = fs.start();
		if (ret < 0) {
			LOG_ERROR << "�ļ�����������ʧ��";
			CCrossTimeServer::m_logFile->flush();
			return -1;
		}
		int fd = 1;
		while (1) {
			proc->RecvFD(fd);
			if (fd <= 0) {
				LOG_INFO << "���յ������ļ�������  fd:" << fd;
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
	/// ���캯��
	/// </summary>
	/// <param name="TPool_cnt">�̳߳��е��߳�����</param>
	/// <param name="sock_lis_cnt">�׽��ּ���ʱ���õ�����</param>
	/// <param name="sock_port">�������˿�</param>
	/// <param name="sock_bind_ip">������IP��ַ</param>
	CFilesServer(
		int TPool_cnt = 4,
		int sock_lis_cnt = 5,
		short sock_port = FILESERVER_PORT,
		const std::string& sock_bind_ip = SERVER_IP
	);
	/// <summary>
	/// ��������
	/// </summary>
	~CFilesServer();
	/// <summary>
	/// ����������
	/// </summary>
	/// <returns>����ֵ��ʾ�������Ƿ����ɹ� ����0��ʾ�ɹ�  -1��ʾʧ��</returns>
	int start();
private:
	typedef int(CFilesServer::* DIS_FUNC)(const UrlParser&,
		const CHttpParser&, std::string&,
		std::string&,
		SOCKET);                                                     // ��Ա�����ĺ���ָ��


	/// <summary>
	/// �������׽��ֳ�ʼ��
	/// </summary>
	/// <returns>����ֵ��ʾ��ʼ���Ƿ�ɹ� 0��ʾ�ɹ� -1��ʾʧ��</returns>
	int sockInit();
	/// <summary>
	/// �ر��׽���
	/// </summary>
	void socketClose();
	/// <summary>
	/// ��ʼ������ӳ���
	/// </summary>
	void initFuncMapping();
	/// <summary>
	/// ����Ͷ��
	/// </summary>
	/// <returns>����ֵΪ0 ��ʾ����Ͷ�ݳɹ�</returns>
	int Dispose();
	/// <summary>
	/// �߳�������
	/// </summary>
	/// <param name="argc">���ݸ��̺߳����Ĳ���</param>
	/// <returns></returns>
	static int MyWorker(void* argc);
	/// <summary>
	/// �߳�������������
	/// </summary>
	void mainWorker();
	/// <summary>
	/// �����׽���Ϊ������ģʽ
	/// </summary>
	/// <param name="fd">Ҫ���÷��������ļ�������</param>
	/// <returns>����ֵ��ʾ�Ƿ����óɹ�  ����0���ʾ���óɹ�  ����ֵΪ-1���ʾ����ʧ��</returns>
	int setNoblock(int fd);
	/// <summary>
	/// HTTPЭ���ʽ���ݽ���
	/// </summary>
	int HttpParser(const CHttpParser& parser, std::string& header, std::string& bodyData, SOCKET clientSocket);
	/// <summary>
	/// ����httpӦ���
	/// </summary>
	/// <param name="bodyData">Ҫ���ظ��û�����Ӧ��</param>
	/// <returns>����httpӦ���������</returns>
	Buffer MakeAckPacket(std::string& header, std::string& bodyData);
private:
	// ҵ������
	// ��ȡָ�������ļ���ҵ����
	int getImageDispose(
		const UrlParser& url,
		const CHttpParser& parser,
		std::string& header,
		std::string& bodyData,
		SOCKET clientSocket = -1);
private:
	CThreadPool                  m_ThreadPool;    // �̳߳�
	CEpoll                       m_ep;            // epoll
	int                          m_serverSock;    // �������׽���
	int                          m_listenCnt;     // �׽��ּ�����
	short                        m_serverPort;    // �׽��ֶ˿�
	std::string                  m_serverIp;      // ������IP��ַ
	std::map<int, FCLIENTINFO*>  m_mapClient;     // ÿ�����������Ŀͻ��˶�Ӧһ��������buf
	std::map<std::string, DIS_FUNC>         m_mGETUriDisfunc;        // GET������ӳ���  uri(GET)->dispose
	std::map<std::string, DIS_FUNC>         m_mPOSTUriDisfunc;       // POST������ӳ��� uri(POST)->dispose
	std::map<std::string, DIS_FUNC>         m_mDELETEUriDisfunc;     // DELETE������ӳ��� uri(DELETE)->dispose
	std::map<std::string, std::string> m_mImageData; // ͼƬ����ӳ���
	std::mutex          m_ImageDatalock;   // ��������д������
};

