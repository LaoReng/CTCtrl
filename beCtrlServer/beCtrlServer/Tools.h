#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <atlstr.h>
#include <atlimage.h>
#include "opencv2/opencv.hpp"

#ifdef _DEBUG  // 调试环境
#pragma comment(lib, "opencv_world490d.lib")
#else         // 发布环境
#pragma comment(lib, "opencv_world490.lib")
#endif

#ifndef TRACE
#ifdef DEBUG   // {解决output变量重定义问题
#define TRACE(str,...) {\
	CString output;\
	output.Format(L"%s(%d):" str, __FILE__, __LINE__, ##__VA_ARGS__);\
	OutputDebugStringW(output);}
#else
// __noop宏禁用
#define TRACE   __noop
#endif // DEBUG
#endif // !TRACE

#define DEV  0

class CTools
{
public:
	/**
	* @brief 获取应用的所在路径（绝对路径）宽字节
	* @param bExeName 表示是否保留后面的程序名(default: false)  false不保留 true保留
	*/
	static std::wstring GetAppPathW(bool bExeName = false) {
		wchar_t szPath[1024] = { '\0' };
		GetModuleFileNameW(NULL, szPath, 1024);
		std::wstring strPath(szPath);
		if (bExeName == false) {
			for (int nPos = (int)strPath.size() - 1; nPos >= 0; --nPos) {
				TCHAR cChar = strPath[nPos];
				// 去掉后面的程序应用名
				if (_T('\\') == cChar || _T('/') == cChar)
					return strPath.substr(0, nPos + 1);
			}
		}
		return strPath;
	}
	/**
	* @brief 获取应用的所在路径（绝对路径）
	* @param bExeName 表示是否保留后面的程序名(default: false)  false不保留 true保留
	*/
	static std::string GetAppPathA(bool bExeName = false) {
		char szPath[1024] = { '\0' };
		GetModuleFileNameA(NULL, szPath, 1024);
		std::string strPath(szPath);
		if (bExeName == false) {
			for (int nPos = (int)strPath.size() - 1; nPos >= 0; --nPos) {
				char cChar = strPath[nPos];
				// 去掉后面的程序应用名
				if ('\\' == cChar || '/' == cChar)
					return strPath.substr(0, nPos + 1);
			}
		}
		return strPath;
	}
	// 获取程序名
	static std::string GetExeNameA() {
		char szPath[1024] = { '\0' };
		GetModuleFileNameA(NULL, szPath, 1024);
		std::string strPath(szPath);
		std::string exeName;
		for (int nPos = (int)strPath.size() - 1; nPos >= 0; --nPos) {
			if ('\\' == strPath[nPos] || '/' == strPath[nPos]) {
				break;
			}
			exeName = strPath[nPos] + exeName;
		}
		return exeName;
	}
	// 加载ICON图标资源
	static HICON LoadIconRes(const std::wstring& IconPath) {
		// 加载图标资源
		return (HICON)::LoadImage(NULL, IconPath.c_str(), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	}
	// 释放ICON图标资源
	static BOOL DestroyIconRes(HICON hIcon) {
		if (hIcon == NULL)return FALSE;
		return DestroyIcon(hIcon);  // 销毁图标资源
	}
	// 宽字节字符转换为正常的字符串（1字节）
	static const std::string wcharStr2charStr(const CString& wstr) {
		std::string str;
		int wstrSize = wstr.GetLength();
		if (wstrSize <= 0)return "";
		int strSize = WideCharToMultiByte(CP_UTF8, 0, wstr.GetString(), wstrSize, NULL, 0, NULL, NULL);
		if (strSize <= 0)return "";
		str.resize(strSize);
		WideCharToMultiByte(CP_UTF8, 0, wstr.GetString(), wstrSize, (char*)str.c_str(), strSize, NULL, NULL);
		return str;
	}
	// 正常的字符（1字节）转换为宽字节字符串
	static const CString charStr2wcharStr(const char* str) {
		CString wstr;
		size_t wideStrLen = std::mbstowcs(NULL, str, 0) + 1;
		wchar_t _wstr[512] = L"";
		std::mbstowcs(_wstr, str, wideStrLen);
		wstr.Format(_T("%s"), _wstr);
		return wstr;
	}
	// 应用重启
	static int Restart() {
		FILE* pf;
		::DeleteFile(L"./restart.cmd");
		errno_t err = ::_wfopen_s(&pf, L"./restart.cmd", L"w");
		if (pf == NULL) {
			return -1;
		}
		if (err == 0) {
			// 获取当前程序的绝对路径
			fprintf(pf, "@echo off\n:a\n taskkill /f /im %s \n start \"\" \"%s\" \ndel %%0", GetExeNameA().c_str(), GetAppPathA(true).c_str());
			fclose(pf);
		}
		else {
			std::string str = "can not open restart.cmd";
			OutputDebugStringA(str.c_str());
			return -1;
		}
		STARTUPINFO si = {};
		PROCESS_INFORMATION pi = {};
		memset(&si, 0, sizeof(si));
		memset(&pi, 0, sizeof(pi));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		// 找到cmd程序的位置
		TCHAR winSysDir[1024] = { '\0' };
		::GetSystemDirectory(winSysDir, 1024);
		std::wstring appName = winSysDir;
		appName += L"\\cmd.exe";
		BOOL bRet = CreateProcess(appName.c_str(), (LPWSTR)L"/c restart.cmd", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		if (bRet == FALSE) {
			int err = GetLastError();
			std::string str(256, '\0');
			snprintf((char*)str.c_str(), str.size(), "Can not restart test, error code:%d", GetLastError());
			OutputDebugStringA(str.c_str());
			return -1;
		}
		return 0;
	}
	/// <summary>
	/// 屏幕截图
	/// </summary>
	/// <param name="imageData">保存图片的缓冲区</param>
	/// <param name="recv">指定截图区域</param>
	/// <returns>返回0，则表示截图成功；否则表示截图失败</returns>
	static int screenCapture(std::string& imageData, cv::Rect& recv) {
		/// 利用CImage类实现屏幕截图
		HDC hDC = GetDC(NULL);
		// 在创建时，创建指定尺寸的Image
		CImage image;
		if (image.Create(
			recv.width,
			recv.height,
			GetDeviceCaps(hDC, BITSPIXEL)) == FALSE)
		{
			TRACE("图片创建失败");
			return -1;
		}
		// 复制指定区域的屏幕，到Image中
		if (BitBlt(
			image.GetDC(),
			0, 0,
			image.GetWidth(), image.GetHeight(),
			hDC, 0, 0,
			SRCCOPY) == FALSE)
		{
			TRACE("图片复制失败");
			image.ReleaseDC();
			return -2;
		}
		ReleaseDC(WindowFromDC(hDC), hDC);
		// 创建图片数据的缓冲区
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
		if (hMem == NULL) {
			image.ReleaseDC();
			return -3;
		}
		IStream* pStream = NULL;
		HRESULT ret = CreateStreamOnHGlobal( // 缓冲区与输入流绑定
			hMem,
			TRUE,
			&pStream);
		if (ret != S_OK) {
			TRACE("内存绑定创建失败");
			GlobalFree(hMem);
			image.ReleaseDC();
			return -4;
		}
		// 保存图片到流中
		image.Save(pStream, Gdiplus::ImageFormatJPEG);
		image.ReleaseDC();
		LARGE_INTEGER bg = {};
		pStream->Seek(bg, STREAM_SEEK_SET, NULL); // 流指针移到开头
		PBYTE pData = (PBYTE)GlobalLock(hMem);    // 将数据锁定提取
		if (pData == NULL) {
			pStream->Release();
			GlobalFree(hMem);
			TRACE("数据大小为0\r\n");
			return -5;
		}
		SIZE_T nSize = GlobalSize(hMem);      // 获取数据大小
		do {
			std::string data(nSize, '\0');
			memcpy((char*)data.c_str(), pData, nSize);
			GlobalUnlock(hMem);
			pStream->Release();
			GlobalFree(hMem);
			// 上面是屏幕截图的完整图片

#if 1
			FILE* pFile = fopen("./original.jpg", "wb+"); // 二进制的写
			if (pFile == NULL) {
				TRACE(_T("文件创建失败\r\n"));
				return -1;
			}
			fwrite(data.c_str(), 1, data.size(), pFile);
			fclose(pFile);
#endif // DEV


			//*** 进行图片质量压缩 ***
			// cv::imread() 该函数可以从磁盘中读取一张图片
			cv::_InputArray src_arr((char*)data.c_str(), (int)data.size());
			cv::Mat src_mat = cv::imdecode(src_arr, cv::IMREAD_COLOR);
			{
				std::string str;
				data.swap(str);
			}
			if (src_mat.data == NULL) {
				// 图片加载失败
				TRACE("图片加载失败");
				break;
			}
			// cv::Mat dst_src = src_mat(recv);  // 这里面保存的就是上面的数据
			cv::Mat amplify1;
			cv::resize(src_mat/*dst_src*/, amplify1, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
			std::vector<uchar> pic_buff; // 保存图片的缓冲区
			// 图片质量压缩
			// IMWRITE_JPEG_QUALITY   jpg   IMWRITE_WEBP_QUALITY  webp
			std::vector<int> compression_params = { cv::IMWRITE_JPEG_QUALITY/*图片类型参数*/ ,50/*图片质量参数*/ };  // 图片处理参数
			bool ret = cv::imencode(".jpg", amplify1, pic_buff, compression_params);
			if (ret == false) {
				pic_buff.clear();
				break;
			}
			imageData.resize(pic_buff.size());
			memcpy((char*)imageData.c_str(), pic_buff.data(), imageData.size());
			// pic_buff.clear();
#if 1
			/*FILE* */pFile = fopen("./dispose.jpg", "wb+"); // 二进制的写
			if (pFile == NULL) {
				TRACE(_T("文件创建失败\r\n"));
				return -1;
			}
			fwrite(imageData.c_str(), 1, imageData.size(), pFile);
			fclose(pFile);
#endif // DEV
			// TRACE("降质后的图片所占字节为：%d\r\n", imageData.size());
			cv::destroyAllWindows();
		} while (false);
		return 0;
	}
	/// <summary>
	/// 本地计算机系统关闭操作
	/// </summary>
	/// <param name="message">要在关闭提示对话框中显示的提示信息；默认（关机：系统即将关机）（重启：系统即将重启）</param>
	/// <param name="bRestartSystrm">是否重启计算机；默认为false； true表示是  false表示否</param>
	/// <param name="timeout">设置即将重启的时间，以秒为单位，如果为0则立即重启；默认为60s</param>
	/// <param name="bForceCloseApps">是否强制关闭为保存的应用程序；默认为true  true表示是  false表示否</param>
	/// <param name="reason">关闭系统的原因；默认为0  详见：https://learn.microsoft.com/windows/win32/shutdown/system-shutdown-reason-codes </param>
	/// <returns>返回值表示操作是否成功；true表示成功   false表示失败</returns>
	static bool LocalSystemShutdown(
		const char* message = NULL,
		bool          bRestartSystrm = false,
		unsigned long timeout = 60,
		bool          bForceCloseApps = true,
		unsigned long reason = 0)
	{
		HANDLE hToken; // 处理令牌的句柄
		TOKEN_PRIVILEGES tkp; // 指向令牌结构的指针
		// 获取当前进程令牌句柄，以便我们可以关机
		// 特权
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
			TRACE(_T("OpenProcessToken失败\n"));
			return false;
		}
		// 获取用于关机特权的LUID。
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;  // 要设置一个特权
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		// 获得该进程的关闭权限。
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		// 无法测试AdjustTokenPrivileges的返回值。
		if (GetLastError() != ERROR_SUCCESS) {
			TRACE(_T("AdjustTokenPrivileges启用失败\n"));
			return false;
		}
		if (message == NULL) {
			message = (bRestartSystrm == false) ? "系统即将关机" : "系统即将重启";
		}
		if (TRUE == InitiateSystemShutdownExA(NULL, (char*)message, timeout, bForceCloseApps, bRestartSystrm, reason)) {
			// TRACE(_T("计算机已关机\n"));
		}
		else {
			// TRACE(_T("无法关闭计算机。错误代码：%d\n"), GetLastError());
			return false;
		}

		// 禁用关机特权。
		tkp.Privileges[0].Attributes = 0;
		if (FALSE == AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0)) {
			TRACE(_T("AdjustTokenPrivileges启用失败\n"));
			return false;
		}
		return true;
	}
	/// <summary>
	/// 撤销本地系统关闭操作
	/// </summary>
	/// <returns>撤销操作执行是否成功；true表示成功   false表示失败</returns>
	static bool RevocationLocalSystemShutdown() {
		HANDLE hToken; // 处理令牌的句柄
		TOKEN_PRIVILEGES tkp; // 指向令牌结构的指针
		// 获取当前进程令牌句柄，以便我们可以关机
		// 特权
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
			TRACE(_T("OpenProcessToken失败\n"));
			return false;
		}
		// 获取用于关机特权的LUID。
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;  // 要设置一个特权
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		// 获得该进程的关闭权限。
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		// 无法测试AdjustTokenPrivileges的返回值。
		if (GetLastError() != ERROR_SUCCESS) {
			TRACE(_T("AdjustTokenPrivileges启用失败\n"));
			return false;
		}

		// 操作撤销也需要有SE_SHUTDOWN_NAME权限
		if (FALSE == AbortSystemShutdownA(NULL)) {
			// TRACE(_T("操作撤销失败\n"));
			return false;
		}
		else {
			// TRACE(_T("操作撤销成功\n"));
		}
		// 禁用关机特权。
		tkp.Privileges[0].Attributes = 0;
		if (FALSE == AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0)) {
			TRACE(_T("AdjustTokenPrivileges启用失败\n"));
			return false;
		}
		return true;
	}
	/// <summary>
	/// 将string字符串转化为utf-8格式的编码
	/// </summary>
	/// <param name="str">要转化的string字符串</param>
	/// <returns>输出为utf-8编码格式的字符串</returns>
	static std::string to_utf8(const std::string& str) {
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		if (nwLen <= 0) {
			return "";
		}
		wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}
	/// <summary>
	/// 将utf-8格式的字符串转换为string类型的字符串
	/// </summary>
	/// <param name="utf8_str">要转化的utf-8格式的字符串</param>
	/// <returns>输出为string格式的字符串</returns>
	static std::string from_utf8(const std::string& utf8_str) {
		// Step 1: Convert UTF-8 to WideChar (UTF-16)
		int wideCharLen = ::MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
		if (wideCharLen == 0) {
			throw std::runtime_error("Error in MultiByteToWideChar: " + std::to_string(GetLastError()));
		}
		std::unique_ptr<wchar_t[]> wideCharBuf(new wchar_t[wideCharLen]);
		::MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, wideCharBuf.get(), wideCharLen);
		// Step 2: Convert WideChar (UTF-16) to ANSI/MBCS
		int multiByteLen = ::WideCharToMultiByte(CP_ACP, 0, wideCharBuf.get(), -1, NULL, 0, NULL, NULL);
		if (multiByteLen == 0) {
			throw std::runtime_error("Error in WideCharToMultiByte: " + std::to_string(GetLastError()));
		}
		std::unique_ptr<char[]> multiByteBuf(new char[multiByteLen]);
		::WideCharToMultiByte(CP_ACP, 0, wideCharBuf.get(), -1, multiByteBuf.get(), multiByteLen, NULL, NULL);
		return std::string(multiByteBuf.get());
	}
};

