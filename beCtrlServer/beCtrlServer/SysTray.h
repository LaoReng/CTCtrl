#pragma once
// ϵͳ������
class CSysTray
{
public:
	CSysTray()
		: m_bInvalid(FALSE)
	{
		memset(&m_stNotifyIconData, 0, sizeof(m_stNotifyIconData));
		m_stNotifyIconData.cbSize = sizeof(m_stNotifyIconData);
	}
	~CSysTray() {
		if (m_bInvalid) {
			RemoveTray();
		}
	}
	// ���ϵͳ����
	BOOL AddTray(HWND hCallBackWnd, UINT uCallBackMsg, UINT uID, HICON hIcon, LPCTSTR lpszTip = NULL) { // ���ϵͳ����
		m_bInvalid = TRUE;

		m_stNotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE;
		m_stNotifyIconData.hWnd = hCallBackWnd;
		m_stNotifyIconData.uCallbackMessage = uCallBackMsg;
		m_stNotifyIconData.uID = uID;
		m_stNotifyIconData.hIcon = hIcon;
		if (lpszTip != NULL && _tcslen(lpszTip) > 0)
		{
			m_stNotifyIconData.uFlags |= NIF_TIP;
			_tcsncpy(m_stNotifyIconData.szTip, lpszTip, sizeof(m_stNotifyIconData.szTip) / sizeof(TCHAR));
		}
		return ::Shell_NotifyIcon(NIM_ADD, &m_stNotifyIconData);
	}
	// �޸�ϵͳͼ��
	BOOL ModifyIcon(HICON hIcon, UINT uID, LPCTSTR lpszTip = NULL) {
		m_stNotifyIconData.uFlags = NIF_ICON;
		m_stNotifyIconData.hIcon = hIcon;
		m_stNotifyIconData.uID = uID;
		if (lpszTip != NULL) {
			m_stNotifyIconData.uFlags |= NIF_TIP;
			_tcsncpy(m_stNotifyIconData.szTip, lpszTip, sizeof(m_stNotifyIconData.szTip) / sizeof(TCHAR));
		}
		return ::Shell_NotifyIcon(NIM_MODIFY, &m_stNotifyIconData);
	}
	// �Ƴ�ϵͳ����
	BOOL RemoveTray() {
		m_bInvalid = FALSE;

		return ::Shell_NotifyIcon(NIM_DELETE, &m_stNotifyIconData);
	}
private:
	NOTIFYICONDATA m_stNotifyIconData;
	BOOL m_bInvalid; // ϵͳͼ���Ƿ���Ч  TRUE��Ч FALSE��Ч
};


