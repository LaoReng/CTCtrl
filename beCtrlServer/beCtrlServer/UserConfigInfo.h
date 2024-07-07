#pragma once

#include <iostream>
#include <cassert>
#include "jsoncpp/json.h"


#define SECRET_KEY  0X6C  // ��Ϣ������Կ

// �û���������Ϣ
typedef struct USER_CONFIG_INFO
{
	char szUser[32];   // �û���
	char szPwd[32];    // ����
	char key[32];      // ��Կ�������룩
	bool bRememberPwd; // ��ס���� true��ʾ��ס����
	bool bAutoLogin;   // �Զ���¼ true��ʾ�Զ���¼

	USER_CONFIG_INFO()
		: szUser("")
		, szPwd("")
		, key("")
		, bRememberPwd(false)
		, bAutoLogin(false)
	{}
	USER_CONFIG_INFO(const USER_CONFIG_INFO& uci)
		: bRememberPwd(uci.bRememberPwd)
		, bAutoLogin(uci.bAutoLogin)
	{
		memcpy(szUser, uci.szUser, sizeof(szUser));
		memcpy(szPwd, uci.szPwd, sizeof(szPwd));
		memcpy(key, uci.key, sizeof(key));
	}
	USER_CONFIG_INFO& operator=(const USER_CONFIG_INFO& uci) {
		if (this != &uci) {
			memcpy(szUser, uci.szUser, sizeof(szUser));
			memcpy(szPwd, uci.szPwd, sizeof(szPwd));
			memcpy(key, uci.key, sizeof(key));
			bRememberPwd = uci.bRememberPwd;
			bAutoLogin = uci.bAutoLogin;
		}
		return *this;
	}
}USERCONFIGINFO;

// �û��˺ż�������Ϣ��
class CUserConfigInfo
{
public:
	CUserConfigInfo(void);
	~CUserConfigInfo(void);

	// ���������
	bool AddCfg(const USER_CONFIG_INFO& cfg);
	// �޸�������
	bool ModifyCfg(const USER_CONFIG_INFO& cfg);
	// ���ļ���������
	bool LoadFile(const char* lpszFileName);
	// �����ļ���ָ������
	bool SaveFile(const char* lpszFileName);
	// ��ȡ����ѡ��
	USER_CONFIG_INFO* GetUserCfgInfo();
private:
	/// <summary>
	/// ���ݼ���
	/// </summary>
	/// <param name="lpBuf">Ҫ���ܵ�����</param>
	/// <param name="nLen">���ݵĳ���</param>
	void Encrypt(char* lpBuf, int nLen);
	/// <summary>
	/// ���ݽ���
	/// </summary>
	/// <param name="lpBuf">Ҫ���ܵ�����</param>
	/// <param name="nLen">���ݵĳ���</param>
	void Decrypt(char* lpBuf, int nLen);
	/// <summary>
	/// ������
	/// </summary>
	/// <param name="lpBuf">�������ݵĻ�����</param>
	/// <param name="nSize">ÿ�ζ�ȡ������ֽ���</param>
	/// <param name="nCount">��Ҫ��ȡ��������</param>
	/// <param name="fp">FILE�ṹ����ļ�ָ��</param>
	/// <returns>����ֵ��ʾ��ȡ�����ַ���</returns>
	int _fread(void* lpBuf, size_t nSize, size_t nCount, FILE* fp);
	/// <summary>
	/// д����
	/// </summary>
	/// <param name="lpBuf">Ҫд�����ݵĻ�����</param>
	/// <param name="nSize">ÿ��д�������ֽ���</param>
	/// <param name="nCount">��Ҫд���������</param>
	/// <param name="fp">FILE�ṹ����ļ�ָ��</param>
	/// <returns>����ֵ��ʾд�����������</returns>
	int _fwrite(const void* lpBuf, size_t nSize, size_t nCount, FILE* fd);
private:
	Json::Value       m_cfgInfo;           // �˻�����������Ϣ��json��
	Json::Value       m_loginCfgInfo;      // ��¼��������Ϣ
	Json::Value       m_settingCfgInfo;    // ���õ�������Ϣ
	USER_CONFIG_INFO* m_userCfgInfo;       // �˻���������Ϣ��struct��
};

