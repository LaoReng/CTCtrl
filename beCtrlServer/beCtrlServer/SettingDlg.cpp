// SettingDlg.cpp: 实现文件
//

#include "pch.h"
#include "beCtrlServer.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_SETTING, pParent)
{}

CSettingDlg::~CSettingDlg()
{}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


BOOL CSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	CRect rect;
	GetWindowRect(&rect);
	TRACE(_T("width:%d height:%d\n"), rect.Width(), rect.Height());
	// 458
	// 250
	rect.bottom = rect.top + 250;
	SetWindowPos(NULL, rect.left, rect.top, 810, 450, SWP_NOZORDER | SWP_NOMOVE);
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	scrollinfo.nPage = 10; //设置滑块大小
	scrollinfo.nMax = 72; //设置滚动条的最大位置0–100
	SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// CSettingDlg 消息处理程序


void CSettingDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	int unit = 3;
	scrollinfo.nPage = 10;
	switch (nSBCode)
	{
	case SB_LINEUP:          //Scroll one line up
		scrollinfo.nPos -= 1;
		if (scrollinfo.nPos < scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, unit);
		break;
	case SB_LINEDOWN:           //Scroll one line down
		scrollinfo.nPos += 1;
		if ((scrollinfo.nPos + scrollinfo.nPage) > (UINT)scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -unit);
		break;
	case SB_PAGEUP:            //Scroll one page up.
		// TRACE(_T("------------当前nPos的值%d\n"), scrollinfo.nPos);
		scrollinfo.nPos -= 5;
		// TRACE(_T("------------加完nPos的值%d\n"), scrollinfo.nPos);
		if (scrollinfo.nPos < scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		// TRACE(_T("-------------向上移动了"));
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, unit * 5);
		break;
	case SB_PAGEDOWN:        //Scroll one page down
		// TRACE(_T("当前nPos的值%d\n"), scrollinfo.nPos);
		scrollinfo.nPos += 5;
		
		// TRACE(_T("加完nPos的值%d\n"), scrollinfo.nPos);
		if (scrollinfo.nPos  > (scrollinfo.nMax - (int)scrollinfo.nPage))  //此处一定要注意加上滑块的长度，再作判断
		{
			// TRACE(_T("往里面存的值%d\n"), scrollinfo.nPos);
			scrollinfo.nPage = 13;
			SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
			break;
		}
		// TRACE(_T("向下移动了"));
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -unit * 5);
		break;
	case SB_ENDSCROLL:      //End scroll     
		break;
	case SB_THUMBPOSITION:  //Scroll to the absolute position. The current position is provided in nPos
		break;
	case SB_THUMBTRACK:                  //Drag scroll box to specified position. The current position is provided in nPos
		ScrollWindow(0, (scrollinfo.nPos - nPos) * unit);
		scrollinfo.nPos = nPos;
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		break;
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CSettingDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	/**
	* zDelta
	向下小于0
	向上大于0
	*/
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo);
	if (zDelta == 120)
	{
		OnVScroll(SB_PAGEUP, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
	}
	else if (zDelta == -120)
	{
		// TRACE("cdSize:%d fMask:%d nMin:%d nMax:%d nPage:%d nPos:%d nTrackPos:%d\n", scrollinfo.cbSize, scrollinfo.fMask, scrollinfo.nMin, scrollinfo.nMax, scrollinfo.nPage, scrollinfo.nPos, scrollinfo.nTrackPos);
		OnVScroll(SB_PAGEDOWN, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
	}
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}
