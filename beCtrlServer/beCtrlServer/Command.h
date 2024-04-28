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

// TODO:该类不能赋值，不能创建，后面要做成单例
class CCommand
{
public:
	CCommand(DWORD mainId = ERROR_INVALID_THREAD_ID);
	~CCommand();
	CCommand(DWORD mainId = ERROR_INVALID_THREAD_ID, const std::string& serverIp = "", unsigned short port = 0);
	// 初始化程序
	bool initCmd();
	// 开启服务
	int Start(DWORD mainId = ERROR_INVALID_THREAD_ID);
	// 停止服务
	int Stop();
	// 设置主线程的id
	void setMainThreadId(DWORD mainId);
private:
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
	// 发送图片信息入口函数
	static void imageSendThreadEnt(void* arg);
	// 发送图片信息主函数
	void imageSendThreadMain();
	/**
	* @brief 屏幕图片截取并输出到缓存中
	* @param recv表示截图区域
	*/
	int SaveScreen(std::string& imageData, cv::Rect& recv);
	// 鼠标键盘数据处理
	int MKEventDispose(const MKEvent& mke);
	// 开启屏幕截屏发送线程的运行
	int StartImageSend();
	// 停止屏幕截屏发送线程的运行
	void StopImageSend();
	// 释放资源
	void Close();
private:
	std::string         m_serverIp;        // 服务器的IP地址
	unsigned short      m_serverPort;      // 服务器的端口
	unsigned short      m_imageSendPort;   // 图片信息发送的端口
	HANDLE              m_hCommand;        // 主服务线程的句柄
	std::atomic<bool>   m_bCommandRun;     // 主服务线程是否继续运行
	HANDLE              m_hImageSend;      // 发送图片线程的句柄
	std::atomic<bool>   m_bImageSendStop;  // 是否停止发送  true不停止 false停止
	std::string         m_buffer;          // 接收数据缓冲区
	CMainDlg            m_mainDlg;         // 程序主对话框窗口
	CSocket             m_sock;            // 套接字
	DWORD               m_mainThreadId;    // 主线程的id，用于线程向主线程发送消息
	std::atomic<SOCKET> m_sockSendImage;   // 发送图片的套接字
	CSysTray            m_sysTray;         // 系统托盘
};

