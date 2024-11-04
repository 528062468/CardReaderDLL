
// CardReaderMainDlg.h : ͷ�ļ�
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


// CCardReaderMainDlg �Ի���
class CCardReaderMainDlg : public CDialogEx
{
// ����
public:
	CCardReaderMainDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CARDREADERMAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
