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

#define CONFIG_FILE_NAME "beCtrl.cfg"               // 配置文件保存文件名称
#define SERVER_IP        "118.190.96.96"            // 服务器IP地址
#define SERVER_PORT      9668                       // 服务器端口
#define WS_ADDR          "ws://118.190.96.96:9002"  // 服务器websocket地址

// TODO:该类不能赋值，不能创建，后面要做成单例
class CCommand
{

	friend CLoginDlg;

public:
	CCommand(DWORD mainId = ERROR_INVALID_THREAD_ID, const std::string& serverIp = "", unsigned short port = 0);
	~CCommand();

	// 开启服务
	int Start(DWORD mainId = ERROR_INVALID_THREAD_ID);
	// 停止服务
	int Stop();
	// 设置主线程的id
	void setMainThreadId(DWORD mainId);
private:
	// 初始化程序
	bool initCmd();
	// 连接服务器
	int Connect();
	// 用户设备登录
	bool login();
	// 初始化系统托盘
	bool initSysTray();
	// 服务线程入口函数
	static void commandThreadEnt(void* arg);
	// 服务线程主函数
	void commandThreadMain();
	// ws客户端运行线程
	static void wsClientThread(void* arg);
	// 发送图片信息入口函数
	static void imageSendThreadEnt(void* arg);
	// 发送图片信息主函数
	void imageSendThreadMain();
	// 鼠标键盘数据处理
	int MKEventDispose(const MKEvent& mke);
	// 开启屏幕截屏发送线程的运行
	int StartImageSend();
	// 停止屏幕截屏发送线程的运行
	void StopImageSend();
	// 释放资源
	void Close();
private:  // webSocket
	/// <summary>
	/// ws收到消息的回调函数，用户可在本函数中对数据进行处理
	/// </summary>
	/// <param name="c">ws客户端指针</param>
	/// <param name="hdl">与服务器连接的凭证</param>
	/// <param name="msg">服务器发来的数据</param>
	static void ws_on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg);
	/// <summary>
	/// 第一次连接上服务器的回调函数
	/// </summary>
	/// <param name="hdl">与服务器连接的凭证</param>
	static void ws_on_connect(websocketpp::connection_hdl hdl);
	/// <summary>
	/// 获取指定目录下的文件和文件夹或者是获取卷
	/// </summary>
	/// <param name="dir">指明要获取的指定目录，如果为""则获取卷</param>
	/// <param name="clien">webSocket客户端指针</param>
	/// <returns>返回值为数据，JSON格式</returns>
	static Json::Value GetFiles(const std::string& dir, WebSocketClient* clien);
	/// <summary>
	/// 判断盘符格式是否正确
	/// </summary>
	/// <param name="volumename">原始数据</param>
	/// <param name="format">目标格式</param>
	/// <returns>返回值表示格式是否正确  TRUE表示正确  FALSE表示错误</returns>
	static BOOL VolumeFormat(const char* volumename, const char* format);
	/// <summary>
	/// ws文件上传业务处理
	/// </summary>
	/// <param name="data">来自服务器的json格式数据</param>
	/// <param name="clien">ws客户端指针</param>
	/// <returns>返回值为数据，JSON格式</returns>
	static Json::Value FileUpload(const Json::Value& data, WebSocketClient* clien);
	/// <summary>
	/// ws文件下载业务处理
	/// </summary>
	/// <param name="data">来自服务器的json格式数据</param>
	/// <param name="clien">ws客户端指针</param>
	/// <returns>返回值为数据，JSON格式</returns>
	static Json::Value FileDownload(const Json::Value& data, WebSocketClient* clien);
private:
	std::string             m_serverIp;        // 服务器的IP地址
	std::string             m_imageSendName;   // 监控图片名
	unsigned short          m_serverPort;      // 服务器的端口
	unsigned short          m_imageSendPort;   // 图片信息发送的端口
	HANDLE                  m_hCommand;        // 主服务线程的句柄
	HANDLE                  m_hWsClient;       // webSocket客户端线程的句柄
	std::atomic<bool>       m_bCommandRun;     // 主服务线程是否继续运行
	HANDLE                  m_hImageSend;      // 发送图片线程的句柄
	std::atomic<bool>       m_bImageSendStop;  // 是否停止发送  true不停止 false停止
	std::string             m_buffer;          // 接收数据缓冲区
	CLoginDlg               m_loginDlg;        // 程序登录窗口
	CSettingDlg             m_settingDlg;      // 程序设置窗口
	CSocket                 m_sock;            // 套接字
	DWORD                   m_mainThreadId;    // 主线程的id，用于线程向主线程发送消息
	std::atomic<SOCKET>     m_sockSendImage;   // 发送图片的套接字
	CSysTray                m_sysTray;         // 系统托盘
	CUserConfigInfo         m_userCfgInfo;     // 用户的配置信息
	USER_CONFIG_INFO*       m_pUserCfgInfo;    // 用户的配置信息，指针
	static WebSocketClient  m_wsClient;        // 文件操作的webSocket客户端
};



