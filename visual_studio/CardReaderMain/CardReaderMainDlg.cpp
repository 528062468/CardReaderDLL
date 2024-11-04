
// CardReaderMainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CardReaderMain.h"
#include "CardReaderMainDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCardReaderMainDlg 对话框




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


// CCardReaderMainDlg 消息处理程序

BOOL CCardReaderMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCardReaderMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCardReaderMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCardReaderMainDlg::OnBnClickedBtnReadcardnum()
{
	if (g_hDll == NULL)
	{
		MessageBox(_T("请先加载动态库"), _T("Info"), MB_OK);
		return;
	}
	UpdateData();
	CString data, error;

	if(true)
	{
		BOOL ret = m_pPComMfid->ReadCardNum(m_szPosition, data, error);
		if (ret)
		{
			MessageBox(_T("读取成功") + data);
		}
		else
		{
			MessageBox(_T("读取失败") + error);
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
	// TODO: 在此添加控件通知处理程序代码
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
		MessageBox(_T("卸载成功!"));
	}
}
