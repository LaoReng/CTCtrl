#pragma once

#include <iostream>
#include <cassert>
#include "jsoncpp/json.h"


#define SECRET_KEY  0X6C  // 信息加密密钥

// 用户的配置信息
typedef struct USER_CONFIG_INFO
{
	char szUser[32];   // 用户名
	char szPwd[32];    // 密码
	char key[32];      // 密钥（控制码）
	bool bRememberPwd; // 记住密码 true表示记住密码
	bool bAutoLogin;   // 自动登录 true表示自动登录

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

// 用户账号及配置信息类
class CUserConfigInfo
{
public:
	CUserConfigInfo(void);
	~CUserConfigInfo(void);

	// 添加配置项
	bool AddCfg(const USER_CONFIG_INFO& cfg);
	// 修改配置项
	bool ModifyCfg(const USER_CONFIG_INFO& cfg);
	// 从文件加载配置
	bool LoadFile(const char* lpszFileName);
	// 保存文件到指定配置
	bool SaveFile(const char* lpszFileName);
	// 获取配置选项
	USER_CONFIG_INFO* GetUserCfgInfo();
private:
	/// <summary>
	/// 数据加密
	/// </summary>
	/// <param name="lpBuf">要加密的数据</param>
	/// <param name="nLen">数据的长度</param>
	void Encrypt(char* lpBuf, int nLen);
	/// <summary>
	/// 数据解密
	/// </summary>
	/// <param name="lpBuf">要解密的数据</param>
	/// <param name="nLen">数据的长度</param>
	void Decrypt(char* lpBuf, int nLen);
	/// <summary>
	/// 读数据
	/// </summary>
	/// <param name="lpBuf">保存数据的缓冲区</param>
	/// <param name="nSize">每次读取的最大字节数</param>
	/// <param name="nCount">需要读取的最大次数</param>
	/// <param name="fp">FILE结构体的文件指针</param>
	/// <returns>返回值表示读取到的字符数</returns>
	int _fread(void* lpBuf, size_t nSize, size_t nCount, FILE* fp);
	/// <summary>
	/// 写数据
	/// </summary>
	/// <param name="lpBuf">要写入数据的缓冲区</param>
	/// <param name="nSize">每次写入的最大字节数</param>
	/// <param name="nCount">需要写入的最大次数</param>
	/// <param name="fp">FILE结构体的文件指针</param>
	/// <returns>返回值表示写入的完整次数</returns>
	int _fwrite(const void* lpBuf, size_t nSize, size_t nCount, FILE* fd);
private:
	Json::Value       m_cfgInfo;           // 账户的总配置信息（json）
	Json::Value       m_loginCfgInfo;      // 登录的配置信息
	Json::Value       m_settingCfgInfo;    // 设置的配置信息
	USER_CONFIG_INFO* m_userCfgInfo;       // 账户的配置信息（struct）
};

