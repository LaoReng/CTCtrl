#pragma once


// CSettingDlg 对话框

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 对话框启动时默认调用的函数（模态启动和非模态启动都调用）
	virtual BOOL OnInitDialog();
	// 响应对话框滚动条事件
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// 响应鼠标滚动事件，用来控制对话框页面滚动
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
