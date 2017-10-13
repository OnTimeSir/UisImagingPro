
// ImageProcessorDlg.h : 头文件
#pragma once

#include "ImageReceiver.h"
#include "afxwin.h"

#define WM_REPORT_STATE WM_USER+1
#define WM_SHOW_RESULT WM_USER+2

// CImageProcessorDlg 对话框
class CImageProcessorDlg : public CDialogEx
{
// 构造
public:
	CImageProcessorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGEPROCESSOR_DIALOG };
#endif

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
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnReportState(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnResultGet(WPARAM wparam, LPARAM lparam);

private:
	int GetNum(CString & strData, LPCSTR lpszTag);
	void ReportState(CString report);

private:
	ImageReceiver* m_imageReceiver;
	BITMAP* m_Result;

};
