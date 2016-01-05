// iotDesignDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "iotDesign.h"
#include "iotDesignDlg.h"
#include "windows.h"
#include "stdio.h"
#include <afxinet.h>
#include <afx.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT RFIDThread(LPVOID pParam);
UINT TestThread(LPVOID pParam);
UINT ZigBeeThread(LPVOID pParam);
BOOL send_message(CString ID);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	CSerial SerialManager1;
	CSerial SerialManager2;
// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CiotDesignDlg 对话框




CiotDesignDlg::CiotDesignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CiotDesignDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CiotDesignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CiotDesignDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CiotDesignDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CiotDesignDlg 消息处理程序

BOOL CiotDesignDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CiotDesignDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CiotDesignDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CiotDesignDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CiotDesignDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (SerialManager1.Open(1, 115200))
	{
		AfxBeginThread(ZigBeeThread, (LPVOID)this);
		
	}
	else
		MessageBox(_T("Failed to open the port1!"), _T("Error"));
	// SerialManager2.Close();

	if (SerialManager2.Open(2, 115200))
	{
		AfxBeginThread(RFIDThread, (LPVOID)this);
	}
	else
		MessageBox(_T("Failed to open the port2!"), _T("Error"));
	// SerialManager2.Close();
	// AfxBeginThread(TestThread, (LPVOID)this);

}


/*
	CString reply;
	CString pram;
	CHttp http;
	http.Init();
	pram = "appid=10586&to18651370755&project=nbMH84&signature=8202eb5e44a5519cd0b989a696a60cec";
	http.post("https://api.submail.cn/message/xsend.json", pram, &reply);
	SetDlgItemText(IDC_EDIT1, reply);
*/

/*
		char* lpBuffer = new char[500];
		int nBytesRead = SerialManager.Read(lpBuffer, 500);
		LPCTSTR text = LPCTSTR(lpBuffer);
		SetDlgItemText(IDC_EDIT1, text);
		delete []lpBuffer;
*/

UINT RFIDThread(LPVOID pParam)
{
	CiotDesignDlg *pDlg = (CiotDesignDlg*)pParam;

	char* lpBuffer2 = new char[500];
	while(true)
	{
		int nBytesRead = pDlg->SerialManager2.Read(lpBuffer2, 500);
			
		CString str1(lpBuffer2,34);
		str1.SetAt(29, '\0');
		CString str2 = str1.Mid(19,29);
		// str2.SetAt(28, '\0');
		pDlg->SetDlgItemText(IDC_EDIT1, str2);
		// send_message(str2);
	}
	delete []lpBuffer2;
	return 0;
}
/*
UINT TestThread(LPVOID pParam)
{
	AfxMessageBox("hello", MB_OK);

	return 0;
}
*/
UINT ZigBeeThread(LPVOID pParam)
{
	CiotDesignDlg *pDlg = (CiotDesignDlg*)pParam;
	char* lpBuffer1 = new char[500];
	while(true)
	{
		int nBytesRead = pDlg->SerialManager1.Read(lpBuffer1, 500);	

		char ctemp[500];
		for (int i=0; i<nBytesRead; ++i){
			if(lpBuffer1[i] == 0)
				lpBuffer1[i] = '1';
			ctemp[i] = lpBuffer1[i];
		}
		char c16temp[500];
		sprintf(c16temp, "%x", ctemp);
		CString str1(c16temp, nBytesRead);

		pDlg->SetDlgItemText(IDC_EDIT1, str1);
		// send_message(str2);
	
	}
	delete []lpBuffer1;
	return 0;
}

BOOL send_message(CString ID)
{
	CString strHeaders =
      _T("Content-Type: application/x-www-form-urlencoded");

   // URL-encoded form variables -
   // name = "John Doe", userid = "hithere", other = "P&Q"
   CString strFormData = _T("appid=10586&to=18651370755&project=nbMH84&signature=8202eb5e44a5519cd0b989a696a60cec");
   strFormData.Append("&vars={\"name\":\"");
   strFormData.Append(ID);
   strFormData.Append("\",\"time\":\"");
   CString strTime;
   CTime currentTime = CTime::GetCurrentTime();
   strTime = currentTime.Format("%Y/%m/%d/%X");
   strFormData.Append(strTime);
   strFormData.Append("\"}");
   CInternetSession session;
   CHttpConnection* pConnection = session.GetHttpConnection(_T("api.submail.cn"));
   CHttpFile* pFile =
      pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,
                              _T("message/xsend.json"));
   BOOL result = pFile->SendRequest(strHeaders,
      (LPVOID)(LPCTSTR)strFormData, strFormData.GetLength());
   return true;
}
