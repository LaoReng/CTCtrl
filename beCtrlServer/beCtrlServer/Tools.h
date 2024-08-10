#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <atlstr.h>
#include <atlimage.h>
#include "opencv2/opencv.hpp"

#ifdef _DEBUG  // ���Ի���
#pragma comment(lib, "opencv_world490d.lib")
#else         // ��������
#pragma comment(lib, "opencv_world490.lib")
#endif

#ifndef TRACE
#ifdef DEBUG   // {���output�����ض�������
#define TRACE(str,...) {\
	CString output;\
	output.Format(L"%s(%d):" str, __FILE__, __LINE__, ##__VA_ARGS__);\
	OutputDebugStringW(output);}
#else
// __noop�����
#define TRACE   __noop
#endif // DEBUG
#endif // !TRACE

#define DEV  0

class CTools
{
public:
	/**
	* @brief ��ȡӦ�õ�����·��������·�������ֽ�
	* @param bExeName ��ʾ�Ƿ�������ĳ�����(default: false)  false������ true����
	*/
	static std::wstring GetAppPathW(bool bExeName = false) {
		wchar_t szPath[1024] = { '\0' };
		GetModuleFileNameW(NULL, szPath, 1024);
		std::wstring strPath(szPath);
		if (bExeName == false) {
			for (int nPos = (int)strPath.size() - 1; nPos >= 0; --nPos) {
				TCHAR cChar = strPath[nPos];
				// ȥ������ĳ���Ӧ����
				if (_T('\\') == cChar || _T('/') == cChar)
					return strPath.substr(0, nPos + 1);
			}
		}
		return strPath;
	}
	/**
	* @brief ��ȡӦ�õ�����·��������·����
	* @param bExeName ��ʾ�Ƿ�������ĳ�����(default: false)  false������ true����
	*/
	static std::string GetAppPathA(bool bExeName = false) {
		char szPath[1024] = { '\0' };
		GetModuleFileNameA(NULL, szPath, 1024);
		std::string strPath(szPath);
		if (bExeName == false) {
			for (int nPos = (int)strPath.size() - 1; nPos >= 0; --nPos) {
				char cChar = strPath[nPos];
				// ȥ������ĳ���Ӧ����
				if ('\\' == cChar || '/' == cChar)
					return strPath.substr(0, nPos + 1);
			}
		}
		return strPath;
	}
	// ��ȡ������
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
	// ����ICONͼ����Դ
	static HICON LoadIconRes(const std::wstring& IconPath) {
		// ����ͼ����Դ
		return (HICON)::LoadImage(NULL, IconPath.c_str(), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
	}
	// �ͷ�ICONͼ����Դ
	static BOOL DestroyIconRes(HICON hIcon) {
		if (hIcon == NULL)return FALSE;
		return DestroyIcon(hIcon);  // ����ͼ����Դ
	}
	// ���ֽ��ַ�ת��Ϊ�������ַ�����1�ֽڣ�
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
	// �������ַ���1�ֽڣ�ת��Ϊ���ֽ��ַ���
	static const CString charStr2wcharStr(const char* str) {
		CString wstr;
		size_t wideStrLen = std::mbstowcs(NULL, str, 0) + 1;
		wchar_t _wstr[512] = L"";
		std::mbstowcs(_wstr, str, wideStrLen);
		wstr.Format(_T("%s"), _wstr);
		return wstr;
	}
	// Ӧ������
	static int Restart() {
		FILE* pf;
		::DeleteFile(L"./restart.cmd");
		errno_t err = ::_wfopen_s(&pf, L"./restart.cmd", L"w");
		if (pf == NULL) {
			return -1;
		}
		if (err == 0) {
			// ��ȡ��ǰ����ľ���·��
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
		// �ҵ�cmd�����λ��
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
	/// ��Ļ��ͼ
	/// </summary>
	/// <param name="imageData">����ͼƬ�Ļ�����</param>
	/// <param name="recv">ָ����ͼ����</param>
	/// <returns>����0�����ʾ��ͼ�ɹ��������ʾ��ͼʧ��</returns>
	static int screenCapture(std::string& imageData, cv::Rect& recv) {
		/// ����CImage��ʵ����Ļ��ͼ
		HDC hDC = GetDC(NULL);
		// �ڴ���ʱ������ָ���ߴ��Image
		CImage image;
		if (image.Create(
			recv.width,
			recv.height,
			GetDeviceCaps(hDC, BITSPIXEL)) == FALSE)
		{
			TRACE("ͼƬ����ʧ��");
			return -1;
		}
		// ����ָ���������Ļ����Image��
		if (BitBlt(
			image.GetDC(),
			0, 0,
			image.GetWidth(), image.GetHeight(),
			hDC, 0, 0,
			SRCCOPY) == FALSE)
		{
			TRACE("ͼƬ����ʧ��");
			image.ReleaseDC();
			return -2;
		}
		ReleaseDC(WindowFromDC(hDC), hDC);
		// ����ͼƬ���ݵĻ�����
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
		if (hMem == NULL) {
			image.ReleaseDC();
			return -3;
		}
		IStream* pStream = NULL;
		HRESULT ret = CreateStreamOnHGlobal( // ����������������
			hMem,
			TRUE,
			&pStream);
		if (ret != S_OK) {
			TRACE("�ڴ�󶨴���ʧ��");
			GlobalFree(hMem);
			image.ReleaseDC();
			return -4;
		}
		// ����ͼƬ������
		image.Save(pStream, Gdiplus::ImageFormatJPEG);
		image.ReleaseDC();
		LARGE_INTEGER bg = {};
		pStream->Seek(bg, STREAM_SEEK_SET, NULL); // ��ָ���Ƶ���ͷ
		PBYTE pData = (PBYTE)GlobalLock(hMem);    // ������������ȡ
		if (pData == NULL) {
			pStream->Release();
			GlobalFree(hMem);
			TRACE("���ݴ�СΪ0\r\n");
			return -5;
		}
		SIZE_T nSize = GlobalSize(hMem);      // ��ȡ���ݴ�С
		do {
			std::string data(nSize, '\0');
			memcpy((char*)data.c_str(), pData, nSize);
			GlobalUnlock(hMem);
			pStream->Release();
			GlobalFree(hMem);
			// ��������Ļ��ͼ������ͼƬ

#if 1
			FILE* pFile = fopen("./original.jpg", "wb+"); // �����Ƶ�д
			if (pFile == NULL) {
				TRACE(_T("�ļ�����ʧ��\r\n"));
				return -1;
			}
			fwrite(data.c_str(), 1, data.size(), pFile);
			fclose(pFile);
#endif // DEV


			//*** ����ͼƬ����ѹ�� ***
			// cv::imread() �ú������ԴӴ����ж�ȡһ��ͼƬ
			cv::_InputArray src_arr((char*)data.c_str(), (int)data.size());
			cv::Mat src_mat = cv::imdecode(src_arr, cv::IMREAD_COLOR);
			{
				std::string str;
				data.swap(str);
			}
			if (src_mat.data == NULL) {
				// ͼƬ����ʧ��
				TRACE("ͼƬ����ʧ��");
				break;
			}
			// cv::Mat dst_src = src_mat(recv);  // �����汣��ľ������������
			cv::Mat amplify1;
			cv::resize(src_mat/*dst_src*/, amplify1, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
			std::vector<uchar> pic_buff; // ����ͼƬ�Ļ�����
			// ͼƬ����ѹ��
			// IMWRITE_JPEG_QUALITY   jpg   IMWRITE_WEBP_QUALITY  webp
			std::vector<int> compression_params = { cv::IMWRITE_JPEG_QUALITY/*ͼƬ���Ͳ���*/ ,50/*ͼƬ��������*/ };  // ͼƬ�������
			bool ret = cv::imencode(".jpg", amplify1, pic_buff, compression_params);
			if (ret == false) {
				pic_buff.clear();
				break;
			}
			imageData.resize(pic_buff.size());
			memcpy((char*)imageData.c_str(), pic_buff.data(), imageData.size());
			// pic_buff.clear();
#if 1
			/*FILE* */pFile = fopen("./dispose.jpg", "wb+"); // �����Ƶ�д
			if (pFile == NULL) {
				TRACE(_T("�ļ�����ʧ��\r\n"));
				return -1;
			}
			fwrite(imageData.c_str(), 1, imageData.size(), pFile);
			fclose(pFile);
#endif // DEV
			// TRACE("���ʺ��ͼƬ��ռ�ֽ�Ϊ��%d\r\n", imageData.size());
			cv::destroyAllWindows();
		} while (false);
		return 0;
	}
	/// <summary>
	/// ���ؼ����ϵͳ�رղ���
	/// </summary>
	/// <param name="message">Ҫ�ڹر���ʾ�Ի�������ʾ����ʾ��Ϣ��Ĭ�ϣ��ػ���ϵͳ�����ػ�����������ϵͳ����������</param>
	/// <param name="bRestartSystrm">�Ƿ������������Ĭ��Ϊfalse�� true��ʾ��  false��ʾ��</param>
	/// <param name="timeout">���ü���������ʱ�䣬����Ϊ��λ�����Ϊ0������������Ĭ��Ϊ60s</param>
	/// <param name="bForceCloseApps">�Ƿ�ǿ�ƹر�Ϊ�����Ӧ�ó���Ĭ��Ϊtrue  true��ʾ��  false��ʾ��</param>
	/// <param name="reason">�ر�ϵͳ��ԭ��Ĭ��Ϊ0  �����https://learn.microsoft.com/windows/win32/shutdown/system-shutdown-reason-codes </param>
	/// <returns>����ֵ��ʾ�����Ƿ�ɹ���true��ʾ�ɹ�   false��ʾʧ��</returns>
	static bool LocalSystemShutdown(
		const char* message = NULL,
		bool          bRestartSystrm = false,
		unsigned long timeout = 60,
		bool          bForceCloseApps = true,
		unsigned long reason = 0)
	{
		HANDLE hToken; // �������Ƶľ��
		TOKEN_PRIVILEGES tkp; // ָ�����ƽṹ��ָ��
		// ��ȡ��ǰ�������ƾ�����Ա����ǿ��Թػ�
		// ��Ȩ
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
			TRACE(_T("OpenProcessTokenʧ��\n"));
			return false;
		}
		// ��ȡ���ڹػ���Ȩ��LUID��
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;  // Ҫ����һ����Ȩ
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		// ��øý��̵Ĺر�Ȩ�ޡ�
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		// �޷�����AdjustTokenPrivileges�ķ���ֵ��
		if (GetLastError() != ERROR_SUCCESS) {
			TRACE(_T("AdjustTokenPrivileges����ʧ��\n"));
			return false;
		}
		if (message == NULL) {
			message = (bRestartSystrm == false) ? "ϵͳ�����ػ�" : "ϵͳ��������";
		}
		if (TRUE == InitiateSystemShutdownExA(NULL, (char*)message, timeout, bForceCloseApps, bRestartSystrm, reason)) {
			// TRACE(_T("������ѹػ�\n"));
		}
		else {
			// TRACE(_T("�޷��رռ������������룺%d\n"), GetLastError());
			return false;
		}

		// ���ùػ���Ȩ��
		tkp.Privileges[0].Attributes = 0;
		if (FALSE == AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0)) {
			TRACE(_T("AdjustTokenPrivileges����ʧ��\n"));
			return false;
		}
		return true;
	}
	/// <summary>
	/// ��������ϵͳ�رղ���
	/// </summary>
	/// <returns>��������ִ���Ƿ�ɹ���true��ʾ�ɹ�   false��ʾʧ��</returns>
	static bool RevocationLocalSystemShutdown() {
		HANDLE hToken; // �������Ƶľ��
		TOKEN_PRIVILEGES tkp; // ָ�����ƽṹ��ָ��
		// ��ȡ��ǰ�������ƾ�����Ա����ǿ��Թػ�
		// ��Ȩ
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
			TRACE(_T("OpenProcessTokenʧ��\n"));
			return false;
		}
		// ��ȡ���ڹػ���Ȩ��LUID��
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;  // Ҫ����һ����Ȩ
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		// ��øý��̵Ĺر�Ȩ�ޡ�
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		// �޷�����AdjustTokenPrivileges�ķ���ֵ��
		if (GetLastError() != ERROR_SUCCESS) {
			TRACE(_T("AdjustTokenPrivileges����ʧ��\n"));
			return false;
		}

		// ��������Ҳ��Ҫ��SE_SHUTDOWN_NAMEȨ��
		if (FALSE == AbortSystemShutdownA(NULL)) {
			// TRACE(_T("��������ʧ��\n"));
			return false;
		}
		else {
			// TRACE(_T("���������ɹ�\n"));
		}
		// ���ùػ���Ȩ��
		tkp.Privileges[0].Attributes = 0;
		if (FALSE == AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0)) {
			TRACE(_T("AdjustTokenPrivileges����ʧ��\n"));
			return false;
		}
		return true;
	}
	/// <summary>
	/// ��string�ַ���ת��Ϊutf-8��ʽ�ı���
	/// </summary>
	/// <param name="str">Ҫת����string�ַ���</param>
	/// <returns>���Ϊutf-8�����ʽ���ַ���</returns>
	static std::string to_utf8(const std::string& str) {
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		if (nwLen <= 0) {
			return "";
		}
		wchar_t* pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β�� 
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
	/// ��utf-8��ʽ���ַ���ת��Ϊstring���͵��ַ���
	/// </summary>
	/// <param name="utf8_str">Ҫת����utf-8��ʽ���ַ���</param>
	/// <returns>���Ϊstring��ʽ���ַ���</returns>
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

