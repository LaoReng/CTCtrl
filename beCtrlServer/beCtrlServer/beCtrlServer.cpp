// beCtrlServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "beCtrlServer.h"
#include "Command.h"

// 设置无控制台模式
#pragma comment(linker,"/subsystem:windows /entry:mainCRTStartup")    //子系统:窗口 入口点:mainCRTStartup

#ifdef _DEBUG
#pragma comment(lib, "libboost_random-vc142-mt-gd-x64-1_83.lib")
#else
#pragma comment(lib, "libboost_random-vc142-mt-x64-1_83.lib")
#endif

#ifdef _DEBUG
// #define new DEBUG_NEW
#else
#endif
// 唯一的应用程序对象

CWinApp theApp;

int Main()
{
	CCommand cmd(GetCurrentThreadId(), SERVER_IP, SERVER_PORT);
	do {
		int ret = cmd.Start();
		if (ret < 0) {
			TRACE(_T("服务开启失败\n"));
			break;
		}
		MSG msg = {};
		while (::GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == CLOSEAPP) {
				// 程序退出
				TRACE(_T("程序退出事件触发\n"));
				break;
			}
			if (msg.message == RESARTAPP) {
				// 重启应用
				if (CTools::Restart() < 0) {
					MessageBox(NULL, _T("操作执行失败"), _T("错误"), MB_OK | MB_ICONERROR);
					continue;
				}
				TRACE(_T("程序重启事件触发\n"));
				break;
			}
			memset(&msg, 0, sizeof(msg));
		}
	} while (false);
	cmd.Stop();
	return 0;
}

int main(int argc, char* argv[])
{
	int nRetCode = 0;
	HMODULE hModule = ::GetModuleHandle(nullptr);
	if (hModule != nullptr)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// 在此处为应用程序的行为编写代码。
			TRACE(_T("错误: MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			Main();
		}
	}
	else
	{
		// 更改错误代码以符合需要
		TRACE(L"错误: GetModuleHandle 失败\n");
		nRetCode = 1;
	}
	return nRetCode;
}
