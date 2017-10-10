
// ImageProcessorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageProcessor.h"
#include "ImageProcessorDlg.h"
#include "afxdialogex.h"
#include "UISRegkey.h"
#include <mmsystem.h>

#pragma comment(lib, "Winmm.lib")

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageProcessorDlg 对话框
CImageProcessorDlg::CImageProcessorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IMAGEPROCESSOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_finalResult = NULL;
}

void CImageProcessorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageProcessorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CImageProcessorDlg::OnBnClickedOk)
	ON_MESSAGE(WM_REPORT_STATE, OnReportState)
	ON_MESSAGE(WM_FINAL_RESULT, OnFinalResultGet)
END_MESSAGE_MAP()

// CImageProcessorDlg 消息处理程序
BOOL CImageProcessorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码

	//获取当前进程的路径信息
	TCHAR strPath[MAX_PATH];
	GetModuleFileName(NULL, strPath, MAX_PATH);
	CString strFileProcessPath;
	strFileProcessPath.Format("%s", strPath);
	int nIndex = strFileProcessPath.ReverseFind(_T('\\'));
	if (nIndex != -1)
	{
		strFileProcessPath = strFileProcessPath.Left(nIndex + 1);
		SetCurrentDirectory(strFileProcessPath);
	}

	//读取配置文件信息
	CString strFile;
	ReadPathFromRegister(_T("UIS_CONFIG_DATA_PATH"), strFile);
	strFile += _T("\\UI\\En\\LayoutData.xml");
	CFile oFile;
	BOOL ret = oFile.Open(strFile, CFile::modeRead);
	if (!ret) {
		ReportState("Open " + strFile + " error.");
		globalDataStruct.rectImageArea.left = 0;
		globalDataStruct.rectImageArea.top = 0;
		globalDataStruct.rectImageArea.right = 1024;
		globalDataStruct.rectImageArea.bottom = 256;
		globalDataStruct.nBufSize = globalDataStruct.rectImageArea.Width() * globalDataStruct.rectImageArea.Height();
		CString report;
		report.Format(_T("Set image config to default Size(width=%d,height=%d) and buffsize set to %d"), globalDataStruct.rectImageArea.Width(), globalDataStruct.rectImageArea.Height(), globalDataStruct.nBufSize);
		ReportState(report);
	}
	else {
		ReportState("Open " + strFile + " success.");
		int len = oFile.GetLength();
		CString strData;
		TCHAR * buf = new TCHAR[len];
		oFile.Read(buf, len);
		strData = buf;
		delete[] buf;
		oFile.Close();
		int nPos = strData.Find("<m_GraphicsRect");
		strData = strData.Mid(nPos + strlen("<m_rcImageArea"));
		globalDataStruct.rectImageArea.left = GetNum(strData, _T("left=\""));;
		globalDataStruct.rectImageArea.top = GetNum(strData, _T("top=\""));;
		globalDataStruct.rectImageArea.right = GetNum(strData, _T("right=\""));
		globalDataStruct.rectImageArea.bottom = GetNum(strData, _T("bottom=\""));
		globalDataStruct.nBufSize = globalDataStruct.rectImageArea.Width() * globalDataStruct.rectImageArea.Height() * 4;
		CString report;
		report.Format(_T("Set image config to Size(width=%d,height=%d) and buffsize set to %d"), globalDataStruct.rectImageArea.Width(), globalDataStruct.rectImageArea.Height(), globalDataStruct.nBufSize);
		ReportState(report);
	}

	//设置共享内存访问Key
	globalDataStruct.strSharedMemoryKey = _T("UIS4DDataTransfer");
	ReportState("Set shared memory key: " + globalDataStruct.strSharedMemoryKey);

	//创建图像接收对象
	m_imageReceiver = new ImageReceiver(this->m_hWnd);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CImageProcessorDlg::OnPaint()
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

		if (m_finalResult) {
			HBITMAP handle = CreateBitmapIndirect(m_finalResult);
			CDC *pDC = ((CStatic*)GetDlgItem(IDC_STATIC))->GetDC();
			CDC memDC;
			memDC.CreateCompatibleDC(pDC);
			CBitmap *pBmp = CBitmap::FromHandle(handle);
			BITMAP bm;
			pBmp->GetBitmap(&bm);
			memDC.SelectObject(pBmp);
			pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &memDC, 0, 0, SRCCOPY);
			//memDC.DeleteObject();
			ReleaseDC(pDC);	
		}
	}
	else
	{
		CDialogEx::OnPaint();
	}

}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImageProcessorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CImageProcessorDlg::GetNum(CString & strData, LPCSTR lpszTag)
{
	int res = 0;
	return res = atoi(strData.Mid(strData.Find(lpszTag) + _tcslen(lpszTag)));
}

void CImageProcessorDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_imageReceiver->startReceive();
}

LRESULT CImageProcessorDlg::OnReportState(WPARAM wparam, LPARAM lparam)
{
	CListBox* listBox = (CListBox*)GetDlgItem(ID_WIZNEXT);
	CString report = *((CString *)lparam);
	listBox->AddString(report);
	return 0;
}

LRESULT CImageProcessorDlg::OnFinalResultGet(WPARAM wparam, LPARAM lparam)
{
	if(0 == wparam)
	{
		if (NULL != m_finalResult) {
			delete m_finalResult;
		}
		m_finalResult = new BITMAP(*(BITMAP*)(lparam));
	}
	m_imageReceiver->stop();
	return 0;
}

void CImageProcessorDlg::ReportState(CString report)
{
	CListBox* listBox = (CListBox*)GetDlgItem(ID_WIZNEXT);
	listBox->AddString(report);
}


