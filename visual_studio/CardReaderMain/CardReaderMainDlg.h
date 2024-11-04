
// CardReaderMainDlg.h : 头文件
//

#pragma once
namespace PComMfid
{
	const char IID_IPComMfig[] = "IPComMfid";
	interface IPComMfid :public IUnknown
	{
		virtual BOOL __stdcall ReadCardNum(CString csPositionId, CString& csCardNum, CString& csErrInfo) = 0;
	};
}


// CCardReaderMainDlg 对话框
class CCardReaderMainDlg : public CDialogEx
{
// 构造
public:
	CCardReaderMainDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CARDREADERMAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_szPosition;
	afx_msg void OnBnClickedBtnReadcardnum();

	HMODULE  g_hDll;
	PComMfid::IPComMfid* m_pPComMfid;
	afx_msg void OnBnClickedBtnLoadlibrary();
	afx_msg void OnBnClickedBtnFreelibrary();
};
