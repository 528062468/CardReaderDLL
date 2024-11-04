
// CardReaderMainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CardReaderMain.h"
#include "CardReaderMainDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCardReaderMainDlg �Ի���




CCardReaderMainDlg::CCardReaderMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCardReaderMainDlg::IDD, pParent)
	, m_szPosition(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_hDll = NULL;
}

void CCardReaderMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_POSITION, m_szPosition);
}

BEGIN_MESSAGE_MAP(CCardReaderMainDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_READCARDNUM, &CCardReaderMainDlg::OnBnClickedBtnReadcardnum)
	ON_BN_CLICKED(IDC_BTN_LOADLIBRARY, &CCardReaderMainDlg::OnBnClickedBtnLoadlibrary)
	ON_BN_CLICKED(IDC_BTN_FREELIBRARY, &CCardReaderMainDlg::OnBnClickedBtnFreelibrary)
END_MESSAGE_MAP()


// CCardReaderMainDlg ��Ϣ�������

BOOL CCardReaderMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCardReaderMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCardReaderMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCardReaderMainDlg::OnBnClickedBtnReadcardnum()
{
	if (g_hDll == NULL)
	{
		MessageBox(_T("���ȼ��ض�̬��"), _T("Info"), MB_OK);
		return;
	}
	UpdateData();
	CString data, error;

	if(true)
	{
		BOOL ret = m_pPComMfid->ReadCardNum(m_szPosition, data, error);
		if (ret)
		{
			MessageBox(_T("��ȡ�ɹ�") + data);
		}
		else
		{
			MessageBox(_T("��ȡʧ��") + error);
		}
	}
	else
	{
		typedef BOOL(/*_stdcall*/ *ADDPROCS)(CString data, CString& a, CString& b);
		ADDPROCS Adds = (ADDPROCS)GetProcAddress(g_hDll, "ReadCardNum");
		BOOL ret = Adds(m_szPosition, data, error);
	}
}


void CCardReaderMainDlg::OnBnClickedBtnLoadlibrary()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g_hDll)
		return;
	{
		g_hDll = LoadLibrary(_T("PComMfid.dll"));
		if (!g_hDll)
		{
			MessageBox(_T("LoadLibrary fail!"), _T("Tips"), MB_OK);
			return;
		}
		typedef LPVOID(*FUN_CREATE_OBJ)(LPCSTR, LPVOID);
		FUN_CREATE_OBJ pFunCreateObject = (FUN_CREATE_OBJ)GetProcAddress(g_hDll, "CreateObject");
		if (!pFunCreateObject)
		{
			MessageBox(_T("CreateObject GetProcAddress fail!"), _T("Tips"), MB_OK);
			return;
		}
		m_pPComMfid = (PComMfid::IPComMfid*)pFunCreateObject("IPComMfid", NULL);
		if (!m_pPComMfid)
		{
			MessageBox(_T("CreateObject return fail!"), _T("Tips"), MB_OK);
			return;
		}
		MessageBox(_T("LoadLibrary Success"), _T("Tips"), MB_OK);
	}

}


void CCardReaderMainDlg::OnBnClickedBtnFreelibrary()
{
	if (g_hDll)
	{
		FreeLibrary(g_hDll);
		g_hDll = NULL;
		MessageBox(_T("ж�سɹ�!"));
	}
}
