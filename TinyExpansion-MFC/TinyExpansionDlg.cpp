
// TinyExpansionDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "TinyExpansion.h"
#include "TinyExpansionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include<bits/stdc++.h>
#include"MAIN.H"
using namespace std;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTinyExpansionDlg 对话框



CTinyExpansionDlg::CTinyExpansionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TINYEXPANSION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTinyExpansionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTinyExpansionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTinyExpansionDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTinyExpansionDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTinyExpansionDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTinyExpansionDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CTinyExpansionDlg 消息处理程序

BOOL CTinyExpansionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTinyExpansionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTinyExpansionDlg::OnPaint()
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
HCURSOR CTinyExpansionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTinyExpansionDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	CString CFileName;
	CFileDialog opendlg(TRUE, _T("*"), /*_T("*.txt")*/NULL, OFN_OVERWRITEPROMPT, _T("所有文件(*.*;)|*.*||"), NULL);
	if (opendlg.DoModal() == IDOK)
	{
		CFileName = opendlg.GetPathName();
		USES_CONVERSION;
		string filename(W2A(CFileName));
		ifstream ifs;
		ifs.open(filename, ios::in);
		string line;
		string input = "";
		while (!ifs.eof()) {
			getline(ifs, line);
			input += line + "\r\n";
		}
		ifs.close();
		CString CInput;
		CInput = input.c_str();
		SetDlgItemText(IDC_EDIT1, CInput);
		//GetDlgItem(IDC_EDIT_SHOW)->SetWindowText(CInput);

	}

}


void CTinyExpansionDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码



	CString CSavePath;
	//打开文件，获取文件路径名
	TCHAR szPath[_MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.lpszTitle = L"请选择保存路径";
	bi.pszDisplayName = szPath;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;

	LPITEMIDLIST pItemIDList = SHBrowseForFolder(&bi);

	if (pItemIDList)
	{
		if (SHGetPathFromIDList(pItemIDList, szPath))
		{
			CSavePath = szPath;
			CSavePath = CSavePath + L"\\";
		}
		CSavePath = CSavePath + L"source.txt";

		CString CText;
		GetDlgItem(IDC_EDIT1)->GetWindowText(CText);
		CText.Replace(L"\r\n", L"\n");
		USES_CONVERSION;
		string text(W2A(CText));
		string save(W2A(CSavePath));
		ofstream ofs;
		ofs.open(save, ios::out);
		ofs << text;
		ofs.close();
		MessageBox(L"已保存至" + CSavePath);

		//use IMalloc interface for avoiding memory leak  
		IMalloc* pMalloc;
		if (SHGetMalloc(&pMalloc) != NOERROR)
		{
			TRACE(_T("Can't get the IMalloc interface\n"));
		}

		pMalloc->Free(pItemIDList);
		if (pMalloc)
			pMalloc->Release();
		UpdateData(FALSE);
	}

}


void CTinyExpansionDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码

	CString CText;
	GetDlgItem(IDC_EDIT1)->GetWindowText(CText);
	CText.Replace(L"\r\n", L"\n");
	USES_CONVERSION;
	string text(W2A(CText));
	string save = "source.txt";
	ofstream ofs;
	ofs.open(save, ios::out);
	ofs << text;
	ofs.close();
	solve();
	MessageBox(L"语法树已生成");

}


void CTinyExpansionDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码

	string filename = "SyntaxTree.txt";
	ifstream ifs;
	ifs.open(filename, ios::in);
	string line;
	string input = "";
	while (!ifs.eof()) {
		getline(ifs, line);
		input += line + "\r\n";
	}
	ifs.close();
	CString CInput;
	CInput = input.c_str();
	SetDlgItemText(IDC_EDIT2, CInput);

}
