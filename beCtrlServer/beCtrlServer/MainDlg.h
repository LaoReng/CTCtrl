#pragma once

#include <afxdialogex.h>

// CMainDlg 对话框

class CCommand;

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	// 标准构造函数
	CLoginDlg(
		DWORD mainId = ERROR_INVALID_THREAD_ID, 
		void* pCom = nullptr, 
		CWnd* pParent = nullptr);
	// 析构函数
	virtual ~CLoginDlg();

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
	// 设置控件的字体
	void SetItemFont(
		CEdit* control,
		int FontSize = 100,
		LPCTSTR lpszFaceName = _T("微软雅黑"));
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
	afx_msg HBRUSH OnCtlColor(
		CDC* pDC,
		CWnd* pWnd,
		UINT nCtlColor);
	// 当账户输入框失去焦点时触发
	afx_msg void OnEnKillfocusEditAccount();
	// 当密码输入对话框失去焦点时触发
	afx_msg void OnEnKillfocusEditPassword();
	// 点击了取消按钮响应函数
	afx_msg void OnBnClickedCancel();
	// 点击了登录按钮响应函数
	afx_msg void OnBnClickedLogin();
	// 点击了记住密码
	afx_msg void OnBnClickedCekRememberpwd();
	// 点击了自动登录
	afx_msg void OnBnClickedCekAutologin();
public:
	// 获取账户文本框中的内容
	CString GetAccText() const;
	// 获取密码文本框中的内容
	CString GetPwText() const;
	// 获取记住密码复选框的值
	BOOL GetRememberPwd() const;
	// 获取自动登录复选框的值
	BOOL GetAutoLogin() const;
	// 设置账户文本框中的内容
	void SetAccText(const char* t);
	// 设置密码文本框中的内容
	void SetPwText(const char* t);
	// 设置记住密码复选框的值
	void SetRememberPwd(bool b);
	// 设置自动登录复选框的值
	void SetAutoLogin(bool b);
private:
	DWORD     m_mainThreadId;     // 主线程的id，用于向服务器发送消息
	CFont*    m_pOldFont;         // 保存对话框的老字体
	CEdit*    m_pEditAccount;     // 用户电话输入文本框
	CEdit*    m_pEditPassword;    // 账户密码输入文本框
	CButton*  m_pEckAutoLogin;    // 自动登录复选框
	CButton*  m_pEckRememberPwd;  // 记住密码复选框
	bool      m_bEckRememberPwd;  // 记住密码是否勾选
	bool      m_bEckAutoLogin;    // 自动登录是否勾选
	bool      m_bAccTextEmpty;    // 判断用户电话输入文本框内容是否为空
	bool      m_bPwTextEmpty;     // 判断账户密码输入文本框内容是否为空
	CString   m_EditAccountText;  // 账号文本
	CString   m_EditPasswordText; // 密码文本
	CCommand* m_pCommand;         // 控制层对象指针
};
