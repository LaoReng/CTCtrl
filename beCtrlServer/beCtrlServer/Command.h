#pragma once
#include "pch.h"
#include "framework.h"
#include <atomic>
#include "Socket.h"
#include "jsoncpp/json.h"
#include "MainDlg.h"
#include "SettingDlg.h"
#include "Tools.h"
#include "HttpParser.h"
#include "ToastHandler.h"
#include "SysTray.h"
#include "resource.h"
#include "Public.h"
#include "UserConfigInfo.h"
#include "WebSocket.h"

#define CONFIG_FILE_NAME "beCtrl.cfg"               // �����ļ������ļ�����
#define SERVER_IP        "118.190.96.96"            // ������IP��ַ
#define SERVER_PORT      9668                       // �������˿�
#define WS_ADDR          "ws://118.190.96.96:9002"  // ������websocket��ַ

// TODO:���಻�ܸ�ֵ�����ܴ���������Ҫ���ɵ���
class CCommand
{

	friend CLoginDlg;

public:
	CCommand(DWORD mainId = ERROR_INVALID_THREAD_ID, const std::string& serverIp = "", unsigned short port = 0);
	~CCommand();

	// ��������
	int Start(DWORD mainId = ERROR_INVALID_THREAD_ID);
	// ֹͣ����
	int Stop();
	// �������̵߳�id
	void setMainThreadId(DWORD mainId);
private:
	// ��ʼ������
	bool initCmd();
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
	// ws�ͻ��������߳�
	static void wsClientThread(void* arg);
	// ����ͼƬ��Ϣ��ں���
	static void imageSendThreadEnt(void* arg);
	// ����ͼƬ��Ϣ������
	void imageSendThreadMain();
	// ���������ݴ���
	int MKEventDispose(const MKEvent& mke);
	// ������Ļ���������̵߳�����
	int StartImageSend();
	// ֹͣ��Ļ���������̵߳�����
	void StopImageSend();
	// �ͷ���Դ
	void Close();
private:  // webSocket
	/// <summary>
	/// ws�յ���Ϣ�Ļص��������û����ڱ������ж����ݽ��д���
	/// </summary>
	/// <param name="c">ws�ͻ���ָ��</param>
	/// <param name="hdl">����������ӵ�ƾ֤</param>
	/// <param name="msg">����������������</param>
	static void ws_on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg);
	/// <summary>
	/// ��һ�������Ϸ������Ļص�����
	/// </summary>
	/// <param name="hdl">����������ӵ�ƾ֤</param>
	static void ws_on_connect(websocketpp::connection_hdl hdl);
	/// <summary>
	/// ��ȡָ��Ŀ¼�µ��ļ����ļ��л����ǻ�ȡ��
	/// </summary>
	/// <param name="dir">ָ��Ҫ��ȡ��ָ��Ŀ¼�����Ϊ""���ȡ��</param>
	/// <param name="clien">webSocket�ͻ���ָ��</param>
	/// <returns>����ֵΪ���ݣ�JSON��ʽ</returns>
	static Json::Value GetFiles(const std::string& dir, WebSocketClient* clien);
	/// <summary>
	/// �ж��̷���ʽ�Ƿ���ȷ
	/// </summary>
	/// <param name="volumename">ԭʼ����</param>
	/// <param name="format">Ŀ���ʽ</param>
	/// <returns>����ֵ��ʾ��ʽ�Ƿ���ȷ  TRUE��ʾ��ȷ  FALSE��ʾ����</returns>
	static BOOL VolumeFormat(const char* volumename, const char* format);
	/// <summary>
	/// ws�ļ��ϴ�ҵ����
	/// </summary>
	/// <param name="data">���Է�������json��ʽ����</param>
	/// <param name="clien">ws�ͻ���ָ��</param>
	/// <returns>����ֵΪ���ݣ�JSON��ʽ</returns>
	static Json::Value FileUpload(const Json::Value& data, WebSocketClient* clien);
	/// <summary>
	/// ws�ļ�����ҵ����
	/// </summary>
	/// <param name="data">���Է�������json��ʽ����</param>
	/// <param name="clien">ws�ͻ���ָ��</param>
	/// <returns>����ֵΪ���ݣ�JSON��ʽ</returns>
	static Json::Value FileDownload(const Json::Value& data, WebSocketClient* clien);
private:
	std::string             m_serverIp;        // ��������IP��ַ
	std::string             m_imageSendName;   // ���ͼƬ��
	unsigned short          m_serverPort;      // �������Ķ˿�
	unsigned short          m_imageSendPort;   // ͼƬ��Ϣ���͵Ķ˿�
	HANDLE                  m_hCommand;        // �������̵߳ľ��
	HANDLE                  m_hWsClient;       // webSocket�ͻ����̵߳ľ��
	std::atomic<bool>       m_bCommandRun;     // �������߳��Ƿ��������
	HANDLE                  m_hImageSend;      // ����ͼƬ�̵߳ľ��
	std::atomic<bool>       m_bImageSendStop;  // �Ƿ�ֹͣ����  true��ֹͣ falseֹͣ
	std::string             m_buffer;          // �������ݻ�����
	CLoginDlg               m_loginDlg;        // �����¼����
	CSettingDlg             m_settingDlg;      // �������ô���
	CSocket                 m_sock;            // �׽���
	DWORD                   m_mainThreadId;    // ���̵߳�id�������߳������̷߳�����Ϣ
	std::atomic<SOCKET>     m_sockSendImage;   // ����ͼƬ���׽���
	CSysTray                m_sysTray;         // ϵͳ����
	CUserConfigInfo         m_userCfgInfo;     // �û���������Ϣ
	USER_CONFIG_INFO*       m_pUserCfgInfo;    // �û���������Ϣ��ָ��
	static WebSocketClient  m_wsClient;        // �ļ�������webSocket�ͻ���
};



