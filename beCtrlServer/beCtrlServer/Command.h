#pragma once
#include "pch.h"
#include "framework.h"
#include <atlimage.h>
#include <atomic>
#include "Socket.h"
#include "jsoncpp/json.h"
#include "opencv2/opencv.hpp"
#include "MainDlg.h"
#include "Tools.h"
#include "HttpParser.h"
#include "ToastHandler.h"
#include "SysTray.h"
#include "resource.h"
#include "Public.h"

// TODO:���಻�ܸ�ֵ�����ܴ���������Ҫ���ɵ���
class CCommand
{
public:
	CCommand(DWORD mainId = ERROR_INVALID_THREAD_ID);
	~CCommand();
	CCommand(DWORD mainId = ERROR_INVALID_THREAD_ID, const std::string& serverIp = "", unsigned short port = 0);
	// ��ʼ������
	bool initCmd();
	// ��������
	int Start(DWORD mainId = ERROR_INVALID_THREAD_ID);
	// ֹͣ����
	int Stop();
	// �������̵߳�id
	void setMainThreadId(DWORD mainId);
private:
	// ���ӷ�����
	int Connect();
	// �û��豸��¼
	bool login();
	// ��ʼ��ϵͳ����
	bool initSysTray();
	// �����߳���ں���
	static void commandThreadEnt(void* arg);
	// �����߳�������
	void commandThreadMain();
	// ����ͼƬ��Ϣ��ں���
	static void imageSendThreadEnt(void* arg);
	// ����ͼƬ��Ϣ������
	void imageSendThreadMain();
	/**
	* @brief ��ĻͼƬ��ȡ�������������
	* @param recv��ʾ��ͼ����
	*/
	int SaveScreen(std::string& imageData, cv::Rect& recv);
	// ���������ݴ���
	int MKEventDispose(const MKEvent& mke);
	// ������Ļ���������̵߳�����
	int StartImageSend();
	// ֹͣ��Ļ���������̵߳�����
	void StopImageSend();
	// �ͷ���Դ
	void Close();
private:
	std::string         m_serverIp;        // ��������IP��ַ
	unsigned short      m_serverPort;      // �������Ķ˿�
	unsigned short      m_imageSendPort;   // ͼƬ��Ϣ���͵Ķ˿�
	HANDLE              m_hCommand;        // �������̵߳ľ��
	std::atomic<bool>   m_bCommandRun;     // �������߳��Ƿ��������
	HANDLE              m_hImageSend;      // ����ͼƬ�̵߳ľ��
	std::atomic<bool>   m_bImageSendStop;  // �Ƿ�ֹͣ����  true��ֹͣ falseֹͣ
	std::string         m_buffer;          // �������ݻ�����
	CMainDlg            m_mainDlg;         // �������Ի��򴰿�
	CSocket             m_sock;            // �׽���
	DWORD               m_mainThreadId;    // ���̵߳�id�������߳������̷߳�����Ϣ
	std::atomic<SOCKET> m_sockSendImage;   // ����ͼƬ���׽���
	CSysTray            m_sysTray;         // ϵͳ����
};

