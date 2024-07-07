#include "pch.h"
#include "UserConfigInfo.h"

CUserConfigInfo::CUserConfigInfo(void)
{
	m_userCfgInfo = new USER_CONFIG_INFO();
	if (m_userCfgInfo == NULL) {
		// ����
		assert(m_userCfgInfo != NULL);
		TRACE("�ڴ����ʧ��\n");
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
	// ���»���
	memcpy(m_userCfgInfo->szUser, cfg.szUser, sizeof(cfg.szUser));
	memcpy(m_userCfgInfo->szPwd, cfg.szPwd, sizeof(cfg.szPwd));
	memcpy(m_userCfgInfo->key, cfg.key, sizeof(cfg.key));
	m_userCfgInfo->bRememberPwd = cfg.bRememberPwd;
	m_userCfgInfo->bAutoLogin = cfg.bAutoLogin;


	// ��¼����
	m_loginCfgInfo["RememberPwd"] = (cfg.bRememberPwd == true ? 1 : 0);
	m_loginCfgInfo["AutoLogin"] = (cfg.bAutoLogin == true ? 1 : 0);
	m_loginCfgInfo["Account"] = cfg.szUser;
	m_loginCfgInfo["Password"] = cfg.szPwd;

	// ��������
	m_settingCfgInfo["key"] = cfg.key;

	// ����
	m_cfgInfo["loginCfg"] = m_loginCfgInfo;
	m_cfgInfo["settingCfg"] = m_settingCfgInfo;
	return true;
}

bool CUserConfigInfo::ModifyCfg(const USER_CONFIG_INFO& cfg)
{
	// ���»���
	memcpy(m_userCfgInfo->szUser, cfg.szUser, sizeof(cfg.szUser));
	memcpy(m_userCfgInfo->szPwd, cfg.szPwd, sizeof(cfg.szPwd));
	memcpy(m_userCfgInfo->key, cfg.key, sizeof(cfg.key));
	m_userCfgInfo->bRememberPwd = cfg.bRememberPwd;
	m_userCfgInfo->bAutoLogin = cfg.bAutoLogin;


	// ��¼����
	m_loginCfgInfo["RememberPwd"] = (cfg.bRememberPwd == true ? 1 : 0);
	m_loginCfgInfo["AutoLogin"] = (cfg.bAutoLogin == true ? 1 : 0);
	m_loginCfgInfo["Account"] = cfg.szUser;
	m_loginCfgInfo["Password"] = cfg.szPwd;

	// ��������
	m_settingCfgInfo["key"] = cfg.key;

	// ����
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
	// �����ƶ��ķ�ʽ���ļ�
	FILE* fp = fopen(lpszFileName, "rb");
	if (NULL == fp) {
		return false;
	}
	char buffer[1024] = "";
	std::string fields[] = { "loginCfg", "settingCfg" };
	_fread(buffer, 1, sizeof(buffer), fp);
	Json::Reader reader;
	if (false == reader.parse(buffer, m_cfgInfo)) {
		TRACE("���ݽ���ʧ��\r\n");
		m_cfgInfo.clear();
		m_loginCfgInfo.clear();
		m_settingCfgInfo.clear();
		fclose(fp);
		return false;
	}
	for (int i = 0; i < sizeof(fields) / sizeof(*fields); i++) {
		if (false == m_cfgInfo.isMember(fields[i])) {
			// ���ݲ�ƥ��
			TRACE("���ݲ�ƥ��\r\n");
			m_cfgInfo.clear();
			m_loginCfgInfo.clear();
			m_settingCfgInfo.clear();
			fclose(fp);
			return false;
		}
	}
	m_loginCfgInfo = m_cfgInfo[fields[0]];
	m_settingCfgInfo = m_cfgInfo[fields[1]];

	// ��¼������
	std::string lCfg[] = { "RememberPwd", "AutoLogin", "Account", "Password" };
	// ����������
	std::string sCfg[] = { "key" };

	for (int i = 0; i < sizeof(lCfg) / sizeof(*lCfg); i++) {
		if (false == m_loginCfgInfo.isMember(lCfg[i])) {
			// ���ݲ�ƥ��
			TRACE("���ݲ�ƥ��\r\n");
			m_cfgInfo.clear();
			m_loginCfgInfo.clear();
			m_settingCfgInfo.clear();
			fclose(fp);
			return false;
		}
	}

	for (int i = 0; i < sizeof(sCfg) / sizeof(*sCfg); i++) {
		if (false == m_settingCfgInfo.isMember(sCfg[i])) {
			// ���ݲ�ƥ��
			TRACE("���ݲ�ƥ��\r\n");
			m_cfgInfo.clear();
			m_loginCfgInfo.clear();
			m_settingCfgInfo.clear();
			fclose(fp);
			return false;
		}
	}
	// ��¼���ü���
	memcpy(m_userCfgInfo->szUser, m_loginCfgInfo[lCfg[2]].asString().c_str(), sizeof(m_userCfgInfo->szUser));
	memcpy(m_userCfgInfo->szPwd, m_loginCfgInfo[lCfg[3]].asString().c_str(), sizeof(m_userCfgInfo->szPwd));

	// int a = m_settingCfgInfo[lCfg[0]].asBool();
	// int b = m_settingCfgInfo[lCfg[1]].asBool();

	m_userCfgInfo->bRememberPwd = (m_loginCfgInfo[lCfg[0]].asInt() == 1 ? true : false);
	m_userCfgInfo->bAutoLogin = (m_loginCfgInfo[lCfg[1]].asInt() == 1 ? true : false);
	// �������ü���
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
	// ���ļ�
	int nRet = (int)fread(lpBuf, 1, nSize * nCount, fp);
	// ����
	Decrypt((char*)lpBuf, nRet);
	return nRet;
}

int CUserConfigInfo::_fwrite(const void* lpBuf, size_t nSize, size_t nCount, FILE* fp)
{
	const size_t nLen = nSize * nCount;
	char cBuf[512] = "";
	memcpy(cBuf, lpBuf, nLen);
	// ����
	Encrypt(cBuf, (int)nLen);
	return (int)fwrite(cBuf, nSize, nCount, fp);
}
