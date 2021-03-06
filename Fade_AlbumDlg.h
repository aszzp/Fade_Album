
// Fade_AlbumDlg.h: 头文件
//

#pragma once

// CFadeAlbumDlg 对话框
class CFadeAlbumDlg : public CDialogEx
{
// 构造
public:
	CFadeAlbumDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FADE_ALBUM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void Ondraw(void);
	void image_fade(int i);
	void sse4_fade(int i);
	DECLARE_MESSAGE_MAP()
public:
	CString path;
	afx_msg
	void OnBnClickedButton1();
	CStatic pic_window;
	afx_msg void OnBnClickedButton2();
	CStatic pic_table;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnEnChangeEdit1();
};
