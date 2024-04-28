#pragma once

#include <afxdialogex.h>



// CMainDlg 对话框

class CMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(DWORD mainId = ERROR_INVALID_THREAD_ID, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMainDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_LOGIN };
#endif

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	// 托盘消息的响应函数
	afx_msg LRESULT OnTrayiconNoTify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
protected:
	void SetItemFont(CEdit* control, int FontSize = 100, LPCTSTR lpszFaceName = _T("微软雅黑")) {
		CFont font;
		font.CreatePointFont(FontSize, lpszFaceName, NULL);
		control->SetFont(&font);
	}
protected:
	// 对话框初始化时自动调用
	virtual BOOL OnInitDialog();
	// 点击了菜单栏的查看控制码
	afx_msg void OnCtrlcode();
	// 点击了菜单栏的设置
	afx_msg void OnSeting();
	// 点击了菜单栏的重启
	afx_msg void OnResart();
	// 点击了菜单栏的退出
	afx_msg void OnExit();
	// 当账户输入框具有焦点时触发
	afx_msg void OnEnSetfocusEditAccount();
	// 当密码输入对话框具有焦点时触发
	afx_msg void OnEnSetfocusEditPassword();
	// 改变控件中字体的颜色
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	// 当账户输入框失去焦点时触发
	afx_msg void OnEnKillfocusEditAccount();
	// 当密码输入对话框失去焦点时触发
	afx_msg void OnEnKillfocusEditPassword();
	// 点击了取消按钮响应函数
	afx_msg void OnBnClickedCancel();
	// 点击了登录按钮响应函数
	afx_msg void OnBnClickedLogin();
public:
	// 获取账户文本框中的内容
	CString GetAccText() const;
	// 获取密码文本框中的内容
	CString GetPwText() const;
private:
	DWORD  m_mainThreadId;  // 主线程的id，用于向服务器发送消息
	CFont* m_pOldFont;      // 保存对话框的老字体
	CEdit* m_pEditAccount;  // 用户电话输入文本框
	CEdit* m_pEditPassword; // 账户密码输入文本框
	bool   m_bAccTextEmpty; // 判断用户电话输入文本框内容是否为空
	bool   m_bPwTextEmpty;  // 判断账户密码输入文本框内容是否为空
	CString m_EditAccountText;
	CString m_EditPasswordText;
};
