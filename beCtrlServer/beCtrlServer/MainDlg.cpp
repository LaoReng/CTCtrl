// MainDlg.cpp: 实现文件
//

#include "pch.h"
#include "MainDlg.h"
#include "beCtrlServer.h"
#include "afxdialogex.h"
#include <Richedit.h>

#include "Command.h"


// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(
	DWORD mainId /*=ERROR_INVALID_THREAD_ID*/,
	void* pCom    /*=nullptr*/,
	CWnd* pParent /*=nullptr*/
)
	: CDialog(IDD_DLG_LOGIN, pParent)
	, m_mainThreadId(mainId)
	, m_pOldFont(NULL)
	, m_pEditAccount(NULL)
	, m_pEditPassword(NULL)
	, m_pEckAutoLogin(NULL)
	, m_pEckRememberPwd(NULL)
	, m_bEckRememberPwd(false)
	, m_bEckAutoLogin(false)
	, m_bAccTextEmpty(false)
	, m_bPwTextEmpty(false)
	, m_pCommand((CCommand*)pCom)
{ }

CLoginDlg::~CLoginDlg()
{
	m_pOldFont->DeleteObject();
	if (m_pEditAccount != NULL) {
		m_pEditAccount->DestroyWindow();
		m_pEditAccount = NULL;
	}
	if (m_pEditPassword != NULL) {
		m_pEditPassword->DestroyWindow();
		m_pEditPassword = NULL;
	}
	if (m_pEckAutoLogin != NULL) {
		m_pEckAutoLogin->DestroyWindow();
		m_pEckAutoLogin = NULL;
	}
	if (m_pEckRememberPwd != NULL) {
		m_pEckRememberPwd->DestroyWindow();
		m_pEckRememberPwd = NULL;
	}
	DestroyWindow();
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

LRESULT CLoginDlg::OnTrayiconNoTify(WPARAM wParam, LPARAM lParam)
{
	// printf("wParam %lld lParam %lld\n", wParam, lParam);
	if (wParam == 1) {
		if (lParam == WM_RBUTTONUP) {
			// 鼠标右键按下
			TRACE(_T("鼠标右键按下\n"));
			// 弹出菜单
			// 获取当前鼠标位置
			CPoint ptMouse = {};
			GetCursorPos(&ptMouse);
			CMenu menu;
			menu.LoadMenu(IDR_MENUTRAY);
			CMenu* pTrayMenu = menu.GetSubMenu(0);
			SetForegroundWindow(); // 设置为前台窗口，当菜单失去焦点时会自动消失
			if (pTrayMenu != NULL) {
				pTrayMenu->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_RIGHTBUTTON | TPM_VERPOSANIMATION, ptMouse.x, ptMouse.y, this);
			}
			menu.DestroyMenu();
		}
		else if (lParam == WM_LBUTTONDBLCLK) {
			// 鼠标左键双击图片
			TRACE(_T("鼠标左键双击\n"));
		}
	}
	return 0;
}

void CLoginDlg::SetItemFont(CEdit* control, int FontSize, LPCTSTR lpszFaceName)
{
	CFont font;
	font.CreatePointFont(FontSize, lpszFaceName, NULL);
	control->SetFont(&font);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_MESSAGE(WM_TRAYICON_NOTIFY, &CLoginDlg::OnTrayiconNoTify)
	ON_COMMAND(ID_CTRLCODE, &CLoginDlg::OnCtrlcode)
	ON_COMMAND(ID_SETING, &CLoginDlg::OnSeting)
	ON_COMMAND(ID_RESART, &CLoginDlg::OnResart)
	ON_COMMAND(ID_EXIT, &CLoginDlg::OnExit)
	ON_EN_SETFOCUS(IDC_EDIT_ACCOUNT, &CLoginDlg::OnEnSetfocusEditAccount)
	ON_EN_SETFOCUS(IDC_EDIT_PASSWORD, &CLoginDlg::OnEnSetfocusEditPassword)
	ON_WM_CTLCOLOR()
	ON_EN_KILLFOCUS(IDC_EDIT_ACCOUNT, &CLoginDlg::OnEnKillfocusEditAccount)
	ON_EN_KILLFOCUS(IDC_EDIT_PASSWORD, &CLoginDlg::OnEnKillfocusEditPassword)
	ON_BN_CLICKED(ID_CANCEL, &CLoginDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_LOGIN, &CLoginDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_CEK_REMEMBERPWD, &CLoginDlg::OnBnClickedCekRememberpwd)
	ON_BN_CLICKED(IDC_CEK_AUTOLOGIN, &CLoginDlg::OnBnClickedCekAutologin)
END_MESSAGE_MAP()

// CMainDlg 消息处理程序
BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 在此添加额外的初始化

	// 设置对话框为无底部状态栏模式
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	// 保存当前对话框的字体样式，后面要改回去
	m_pOldFont = GetFont();
	// 获取对话框中的账户和密码Edit控件
	m_pEditAccount = (CEdit*)GetDlgItem(IDC_EDIT_ACCOUNT);
	m_pEditPassword = (CEdit*)GetDlgItem(IDC_EDIT_PASSWORD);
	m_pEckAutoLogin = (CButton*)GetDlgItem(IDC_CEK_AUTOLOGIN);
	m_pEckRememberPwd = (CButton*)GetDlgItem(IDC_CEK_REMEMBERPWD);

	// 设置字体的样式
	/*
	CFont font;
	font.CreatePointFont(100, _T("微软雅黑"), NULL);
	m_pEditAccount->SetFont(&font);
	m_pEditPassword->SetFont(&font);*/
	if (m_bAccTextEmpty == false) {
		SetItemFont(m_pEditAccount);
		m_pEditAccount->SetWindowText(_T("请输入电话号码"));
	}
	else {
		m_pEditAccount->SetWindowText(m_EditAccountText);
	}
	if (m_bPwTextEmpty == false) {
		SetItemFont(m_pEditPassword);
		// 设置默认显示文字
		m_pEditPassword->SetWindowText(_T("请输入密码"));
	}
	else {
		m_pEditPassword->SetWindowText(m_EditPasswordText);
		m_pEditPassword->SetPasswordChar('*');
	}
	m_pEckAutoLogin->SetCheck((m_bEckAutoLogin == true ? BST_CHECKED : BST_UNCHECKED));
	m_pEckRememberPwd->SetCheck((m_bEckRememberPwd == true ? BST_CHECKED : BST_UNCHECKED));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CLoginDlg::OnCtrlcode()
{
	TRACE(_T("点击了查看控制码\n"));
	// 弹出一个消息框
	std::wstring firstLine = L"控制码";
	std::wstring secondLine = L"当前设备控制码：\"123456\"";
	CToast::ShowNotification(firstLine, secondLine);
}

void CLoginDlg::OnSeting()
{
	TRACE(_T("点击了设置\n"));
	// 在此添加命令处理程序代码

	// m_pCommand

	m_pCommand->m_settingDlg.DoModal();

}

void CLoginDlg::OnResart()
{
	TRACE(_T("点击了重启"));
	// 先给服务器发送设备下线指令



	PostThreadMessage(m_mainThreadId, RESARTAPP, 0, 0);
	// 在此添加命令处理程序代码
}

void CLoginDlg::OnExit()
{
	TRACE(_T("点击了退出"));





	PostThreadMessage(m_mainThreadId, CLOSEAPP, 0, 0);
	// 在此添加命令处理程序代码
}


void CLoginDlg::OnEnSetfocusEditAccount()
{
	// 在此添加控件通知处理程序代码
	CString str;
	UpdateData();
	m_pEditAccount->GetWindowText(str);
	// GetDlgItemText(IDC_EDIT_ACCOUNT, str);
	if (str != "请输入电话号码")
		return;
	// m_pDC->SetTextColor(RGB(0, 255, 0));
	m_pEditAccount->SetWindowText(_T(""));
	// SetDlgItemText(IDC_EDIT_ACCOUNT, _T(""));
	if (m_pOldFont)
		m_pEditAccount->SetFont(m_pOldFont);
	m_bAccTextEmpty = true;
	UpdateData(FALSE);
}


void CLoginDlg::OnEnSetfocusEditPassword()
{
	// 在此添加控件通知处理程序代码
	CString str;
	UpdateData();
	m_pEditPassword->GetWindowText(str);
	// GetDlgItemText(IDC_EDIT_PASSWORD, str);
	if (str != "请输入密码")
		return;
	m_pEditPassword->SetWindowText(_T(""));
	// SetDlgItemText(IDC_EDIT_PASSWORD, _T(""));
	m_pEditPassword->SetPasswordChar('*');
	if (m_pOldFont)
		m_pEditPassword->SetFont(m_pOldFont);
	m_bPwTextEmpty = true;
	UpdateData(FALSE);
}


HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// 在此更改 DC 的任何特性

	if (pWnd->GetDlgCtrlID() == IDC_EDIT_ACCOUNT) {
		if (m_bAccTextEmpty == false) {
			pDC->SetTextColor(RGB(184, 184, 184));
		}
		else {
			pDC->SetTextColor(RGB(0, 0, 0));
		}
	}
	else if (pWnd->GetDlgCtrlID() == IDC_EDIT_PASSWORD) {
		if (m_bPwTextEmpty == false) {
			pDC->SetTextColor(RGB(184, 184, 184));
			m_pEditPassword->SetPasswordChar(0);
		}
		else {
			pDC->SetTextColor(RGB(0, 0, 0));
		}
	}

	// 如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CLoginDlg::OnEnKillfocusEditAccount()
{
	// 在此添加控件通知处理程序代码
	CString str;
	m_pEditAccount->GetWindowText(str);
	if (str == _T("")) {
		SetItemFont(m_pEditAccount);
		m_pEditAccount->SetWindowText(_T("请输入电话号码"));
		m_bAccTextEmpty = false;
	}
}


void CLoginDlg::OnEnKillfocusEditPassword()
{
	// 在此添加控件通知处理程序代码
	CString str;
	m_pEditPassword->GetWindowText(str);
	if (str == _T("")) {
		SetItemFont(m_pEditPassword);
		m_pEditPassword->SetWindowText(_T("请输入密码"));
		m_bPwTextEmpty = false;
	}
}


void CLoginDlg::OnBnClickedCancel()
{
	// 在此添加控件通知处理程序代码
	m_bAccTextEmpty = false;
	m_bPwTextEmpty = false;
	EndDialog(IDCANCEL);  // DoModal的返回值可以接收到
}


void CLoginDlg::OnBnClickedLogin()
{
	// 在此添加控件通知处理程序代码
	// 登录的时候还要判断，用户名和密码不能为空

	if (m_pEditAccount)
		m_pEditAccount->GetWindowText(m_EditAccountText);
	if (m_pEditPassword)
		m_pEditPassword->GetWindowText(m_EditPasswordText);
	if (m_pEckAutoLogin)
		m_bEckAutoLogin = (m_pEckAutoLogin->GetCheck() == BST_CHECKED ? true : false);
	if (m_pEckRememberPwd)
		m_bEckRememberPwd = (m_pEckRememberPwd->GetCheck() == BST_CHECKED ? true : false);

	if ((m_EditAccountText == "请输入电话号码" || m_EditAccountText == "")
		|| (m_EditPasswordText == "请输入密码" || m_EditPasswordText == ""))
	{
		MessageBox(_T("请输入用户名或密码"), _T("提示"), MB_OK | MB_ICONWARNING);
		return;
	}
	m_bAccTextEmpty = false;
	m_bPwTextEmpty = false;
	EndDialog(IDOK);  // DoModal的返回值可以接收到
}

CString CLoginDlg::GetAccText() const
{
	/*if (m_pEditAccount == NULL)
		return CString(_T(""));
	CString text;
	m_pEditAccount->GetWindowText(text);*/
	return m_EditAccountText;
}

CString CLoginDlg::GetPwText() const
{
	/*if (m_pEditPassword == NULL)
		return CString(_T(""));
	CString text;
	m_pEditPassword->GetWindowText(text);*/
	return m_EditPasswordText;
}

BOOL CLoginDlg::GetRememberPwd() const
{
	return m_bEckRememberPwd;
}

BOOL CLoginDlg::GetAutoLogin() const
{
	return m_bEckAutoLogin;
}

#include "Tools.h"

void CLoginDlg::SetAccText(const char* t)
{

	m_EditAccountText = CTools::charStr2wcharStr(t);
	if (m_EditAccountText != _T("")) {
		m_bAccTextEmpty = true;
	}
}

void CLoginDlg::SetPwText(const char* t)
{
	m_EditPasswordText = CTools::charStr2wcharStr(t);
	if (m_EditPasswordText != _T("")) {
		m_bPwTextEmpty = true;
	}
}

void CLoginDlg::SetRememberPwd(bool b)
{
	m_bEckRememberPwd = b;
}

void CLoginDlg::SetAutoLogin(bool b)
{
	m_bEckAutoLogin = b;
}


void CLoginDlg::OnBnClickedCekRememberpwd()
{
	// 在此添加控件通知处理程序代码
	if (m_pEckAutoLogin == NULL || m_pEckRememberPwd == NULL)
		return;
	// 记住密码为false就要自动取消自动登录的true
	if (m_pEckRememberPwd->GetCheck() == BST_UNCHECKED) {
		// 记住密码为false
		m_pEckAutoLogin->SetCheck(BST_UNCHECKED); // 取消自动登录
	}


	/*if (state == BST_CHECKED) {
		TRACE("点击了记住密码，当前值为TRUE\r\n");
	}
	else if (state == BST_UNCHECKED) {
		TRACE("点击了记住密码，当前值为FALSE\r\n");
	}*/
}


void CLoginDlg::OnBnClickedCekAutologin()
{
	// 在此添加控件通知处理程序代码
	if (m_pEckAutoLogin == NULL || m_pEckRememberPwd == NULL)
		return;
	// 自动登录为true就要自动勾选记住密码的true
	int state = m_pEckAutoLogin->GetCheck();
	if (state == BST_CHECKED) {
		// 记住密码为true
		m_pEckRememberPwd->SetCheck(BST_CHECKED); // 勾选记住密码
	}

	/*if (state == BST_CHECKED) {
		TRACE("点击了自动登录，当前值为TRUE\r\n");
	}
	else if (state == BST_UNCHECKED) {
		TRACE("点击了自动登录，当前值为FALSE\r\n");
	}*/
}
