#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <atlstr.h>

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
	// ���ֽ��ַ�ת��Ϊ�������ַ���1�ֽڣ�
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
};

