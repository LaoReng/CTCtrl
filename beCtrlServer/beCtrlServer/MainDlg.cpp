// MainDlg.cpp: 实现文件
//

#include "pch.h"
#include "beCtrlServer.h"
#include "MainDlg.h"
#include "afxdialogex.h"


// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(DWORD mainId /*=ERROR_INVALID_THREAD_ID*/, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_LOGIN, pParent)
	, m_mainThreadId(mainId)
	, m_pOldFont(NULL)
	, m_pEditAccount(NULL)
	, m_pEditPassword(NULL)
	, m_bAccTextEmpty(false)
	, m_bPwTextEmpty(false)
{ }

CMainDlg::~CMainDlg()
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
	DestroyWindow();
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

LRESULT CMainDlg::OnTrayiconNoTify(WPARAM wParam, LPARAM lParam)
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

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_MESSAGE(WM_TRAYICON_NOTIFY, &CMainDlg::OnTrayiconNoTify)
	ON_COMMAND(ID_CTRLCODE, &CMainDlg::OnCtrlcode)
	ON_COMMAND(ID_SETING, &CMainDlg::OnSeting)
	ON_COMMAND(ID_RESART, &CMainDlg::OnResart)
	ON_COMMAND(ID_EXIT, &CMainDlg::OnExit)
	ON_EN_SETFOCUS(IDC_EDIT_ACCOUNT, &CMainDlg::OnEnSetfocusEditAccount)
	ON_EN_SETFOCUS(IDC_EDIT_PASSWORD, &CMainDlg::OnEnSetfocusEditPassword)
	ON_WM_CTLCOLOR()
	ON_EN_KILLFOCUS(IDC_EDIT_ACCOUNT, &CMainDlg::OnEnKillfocusEditAccount)
	ON_EN_KILLFOCUS(IDC_EDIT_PASSWORD, &CMainDlg::OnEnKillfocusEditPassword)
	ON_BN_CLICKED(ID_CANCEL, &CMainDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_LOGIN, &CMainDlg::OnBnClickedLogin)
END_MESSAGE_MAP()

#include <Richedit.h>


// CMainDlg 消息处理程序
BOOL CMainDlg::OnInitDialog()
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


	// 设置字体的样式
	/*
	CFont font;
	font.CreatePointFont(100, _T("微软雅黑"), NULL);
	m_pEditAccount->SetFont(&font);
	m_pEditPassword->SetFont(&font);*/
	SetItemFont(m_pEditAccount);
	SetItemFont(m_pEditPassword);
	// 设置默认显示文字
	m_pEditAccount->SetWindowText(_T("请输入电话号码"));
	m_pEditPassword->SetWindowText(_T("请输入密码"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CMainDlg::OnCtrlcode()
{
	TRACE(_T("点击了查看控制码\n"));
	// 在此添加命令处理程序代码
}

void CMainDlg::OnSeting()
{
	TRACE(_T("点击了设置\n"));
	// 在此添加命令处理程序代码
}

void CMainDlg::OnResart()
{
	TRACE(_T("点击了重启"));
	PostThreadMessage(m_mainThreadId, RESARTAPP, 0, 0);
	// 在此添加命令处理程序代码
}

void CMainDlg::OnExit()
{
	TRACE(_T("点击了退出"));
	PostThreadMessage(m_mainThreadId, CLOSEAPP, 0, 0);
	// 在此添加命令处理程序代码
}


void CMainDlg::OnEnSetfocusEditAccount()
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


void CMainDlg::OnEnSetfocusEditPassword()
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


HBRUSH CMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


void CMainDlg::OnEnKillfocusEditAccount()
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


void CMainDlg::OnEnKillfocusEditPassword()
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


void CMainDlg::OnBnClickedCancel()
{
	// 在此添加控件通知处理程序代码
	m_bAccTextEmpty = false;
	m_bPwTextEmpty = false;
	EndDialog(IDCANCEL);
}


void CMainDlg::OnBnClickedLogin()
{
	// 在此添加控件通知处理程序代码
	// 登录的时候还要判断，用户名和密码不能为空

	if (m_pEditAccount)
		m_pEditAccount->GetWindowText(m_EditAccountText);
	if (m_pEditPassword)
		m_pEditPassword->GetWindowText(m_EditPasswordText);

	if ((m_EditAccountText == "请输入电话号码" || m_EditAccountText == "")
		|| (m_EditPasswordText == "请输入密码" || m_EditPasswordText == ""))
	{
		MessageBox(_T("请输入用户名或密码"), _T("提示"), MB_OK | MB_ICONWARNING);
		return;
	}
	m_bAccTextEmpty = false;
	m_bPwTextEmpty = false;
	EndDialog(IDOK);
}

CString CMainDlg::GetAccText() const
{
	/*if (m_pEditAccount == NULL)
		return CString(_T(""));
	CString text;
	m_pEditAccount->GetWindowText(text);*/
	return m_EditAccountText;
}

CString CMainDlg::GetPwText() const
{
	/*if (m_pEditPassword == NULL)
		return CString(_T(""));
	CString text;
	m_pEditPassword->GetWindowText(text);*/
	return m_EditPasswordText;
}
