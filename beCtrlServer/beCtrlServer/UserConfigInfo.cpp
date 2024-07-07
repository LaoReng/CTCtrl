#include "pch.h"
#include "UserConfigInfo.h"

CUserConfigInfo::CUserConfigInfo(void)
{
	m_userCfgInfo = new USER_CONFIG_INFO();
	if (m_userCfgInfo == NULL) {
		// 断言
		assert(m_userCfgInfo != NULL);
		TRACE("内存分配失败\n");
	}
}

CUserConfigInfo::~CUserConfigInfo(void)
{
	if (m_userCfgInfo != NULL) {
		USER_CONFIG_INFO* temp = m_userCfgInfo;
		m_userCfgInfo = NULL;
		delete temp;
	}
}

bool CUserConfigInfo::AddCfg(const USER_CONFIG_INFO& cfg)
{
	// 更新缓存
	memcpy(m_userCfgInfo->szUser, cfg.szUser, sizeof(cfg.szUser));
	memcpy(m_userCfgInfo->szPwd, cfg.szPwd, sizeof(cfg.szPwd));
	memcpy(m_userCfgInfo->key, cfg.key, sizeof(cfg.key));
	m_userCfgInfo->bRememberPwd = cfg.bRememberPwd;
	m_userCfgInfo->bAutoLogin = cfg.bAutoLogin;


	// 登录配置
	m_loginCfgInfo["RememberPwd"] = (cfg.bRememberPwd == true ? 1 : 0);
	m_loginCfgInfo["AutoLogin"] = (cfg.bAutoLogin == true ? 1 : 0);
	m_loginCfgInfo["Account"] = cfg.szUser;
	m_loginCfgInfo["Password"] = cfg.szPwd;

	// 设置配置
	m_settingCfgInfo["key"] = cfg.key;

	// 汇总
	m_cfgInfo["loginCfg"] = m_loginCfgInfo;
	m_cfgInfo["settingCfg"] = m_settingCfgInfo;
	return true;
}

bool CUserConfigInfo::ModifyCfg(const USER_CONFIG_INFO& cfg)
{
	// 更新缓存
	memcpy(m_userCfgInfo->szUser, cfg.szUser, sizeof(cfg.szUser));
	memcpy(m_userCfgInfo->szPwd, cfg.szPwd, sizeof(cfg.szPwd));
	memcpy(m_userCfgInfo->key, cfg.key, sizeof(cfg.key));
	m_userCfgInfo->bRememberPwd = cfg.bRememberPwd;
	m_userCfgInfo->bAutoLogin = cfg.bAutoLogin;


	// 登录配置
	m_loginCfgInfo["RememberPwd"] = (cfg.bRememberPwd == true ? 1 : 0);
	m_loginCfgInfo["AutoLogin"] = (cfg.bAutoLogin == true ? 1 : 0);
	m_loginCfgInfo["Account"] = cfg.szUser;
	m_loginCfgInfo["Password"] = cfg.szPwd;

	// 设置配置
	m_settingCfgInfo["key"] = cfg.key;

	// 汇总
	m_cfgInfo["loginCfg"] = m_loginCfgInfo;
	m_cfgInfo["settingCfg"] = m_settingCfgInfo;
	return true;
}

bool CUserConfigInfo::LoadFile(const char* lpszFileName)
{
	if (m_userCfgInfo == NULL) {
		return false;
	}
	if (NULL == lpszFileName || NULL == *lpszFileName) {
		return false;
	}
	// 二进制读的方式打开文件
	FILE* fp = fopen(lpszFileName, "rb");
	if (NULL == fp) {
		return false;
	}
	char buffer[1024] = "";
	std::string fields[] = { "loginCfg", "settingCfg" };
	_fread(buffer, 1, sizeof(buffer), fp);
	Json::Reader reader;
	if (false == reader.parse(buffer, m_cfgInfo)) {
		TRACE("数据解析失败\r\n");
		m_cfgInfo.clear();
		m_loginCfgInfo.clear();
		m_settingCfgInfo.clear();
		fclose(fp);
		return false;
	}
	for (int i = 0; i < sizeof(fields) / sizeof(*fields); i++) {
		if (false == m_cfgInfo.isMember(fields[i])) {
			// 数据不匹配
			TRACE("数据不匹配\r\n");
			m_cfgInfo.clear();
			m_loginCfgInfo.clear();
			m_settingCfgInfo.clear();
			fclose(fp);
			return false;
		}
	}
	m_loginCfgInfo = m_cfgInfo[fields[0]];
	m_settingCfgInfo = m_cfgInfo[fields[1]];

	// 登录配置项
	std::string lCfg[] = { "RememberPwd", "AutoLogin", "Account", "Password" };
	// 设置配置项
	std::string sCfg[] = { "key" };

	for (int i = 0; i < sizeof(lCfg) / sizeof(*lCfg); i++) {
		if (false == m_loginCfgInfo.isMember(lCfg[i])) {
			// 数据不匹配
			TRACE("数据不匹配\r\n");
			m_cfgInfo.clear();
			m_loginCfgInfo.clear();
			m_settingCfgInfo.clear();
			fclose(fp);
			return false;
		}
	}

	for (int i = 0; i < sizeof(sCfg) / sizeof(*sCfg); i++) {
		if (false == m_settingCfgInfo.isMember(sCfg[i])) {
			// 数据不匹配
			TRACE("数据不匹配\r\n");
			m_cfgInfo.clear();
			m_loginCfgInfo.clear();
			m_settingCfgInfo.clear();
			fclose(fp);
			return false;
		}
	}
	// 登录配置加载
	memcpy(m_userCfgInfo->szUser, m_loginCfgInfo[lCfg[2]].asString().c_str(), sizeof(m_userCfgInfo->szUser));
	memcpy(m_userCfgInfo->szPwd, m_loginCfgInfo[lCfg[3]].asString().c_str(), sizeof(m_userCfgInfo->szPwd));

	// int a = m_settingCfgInfo[lCfg[0]].asBool();
	// int b = m_settingCfgInfo[lCfg[1]].asBool();

	m_userCfgInfo->bRememberPwd = (m_loginCfgInfo[lCfg[0]].asInt() == 1 ? true : false);
	m_userCfgInfo->bAutoLogin = (m_loginCfgInfo[lCfg[1]].asInt() == 1 ? true : false);
	// 设置配置加载
	memcpy(m_userCfgInfo->key, m_settingCfgInfo[sCfg[0]].asString().c_str(), sizeof(m_userCfgInfo->key));

	fclose(fp);

	return true;
}

bool CUserConfigInfo::SaveFile(const char* lpszFileName)
{
	if (NULL == lpszFileName || NULL == *lpszFileName) {
		return false;
	}

	FILE* fp = fopen(lpszFileName, "wb+");
	if (fp == NULL) {
		return false;
	}

	ModifyCfg(*m_userCfgInfo);
	std::string buf = m_cfgInfo.toStyledString();

	_fwrite(buf.c_str(), 1, buf.size(), fp);

	fclose(fp);

	return true;
}

USER_CONFIG_INFO* CUserConfigInfo::GetUserCfgInfo()
{

	return m_userCfgInfo;
}

void CUserConfigInfo::Encrypt(char* lpBuf, int nLen)
{
	if (lpBuf == NULL)return;
	for (int i = 0; i < nLen; i++) {
		lpBuf[i] = lpBuf[i] ^ SECRET_KEY;
	}
}

void CUserConfigInfo::Decrypt(char* lpBuf, int nLen)
{
	if (lpBuf == NULL)return;
	for (int i = 0; i < nLen; i++) {
		lpBuf[i] = lpBuf[i] ^ SECRET_KEY;
	}
}

int CUserConfigInfo::_fread(void* lpBuf, size_t nSize, size_t nCount, FILE* fp)
{
	// 读文件
	int nRet = (int)fread(lpBuf, 1, nSize * nCount, fp);
	// 解密
	Decrypt((char*)lpBuf, nRet);
	return nRet;
}

int CUserConfigInfo::_fwrite(const void* lpBuf, size_t nSize, size_t nCount, FILE* fp)
{
	const size_t nLen = nSize * nCount;
	char cBuf[512] = "";
	memcpy(cBuf, lpBuf, nLen);
	// 加密
	Encrypt(cBuf, (int)nLen);
	return (int)fwrite(cBuf, nSize, nCount, fp);
}
