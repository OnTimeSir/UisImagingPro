
// ImageProcessorDlg.h : ͷ�ļ�
#pragma once

#include "ImageReceiver.h"
#include "afxwin.h"

#define WM_REPORT_STATE WM_USER+1
#define WM_FINAL_RESULT WM_USER+2

// CImageProcessorDlg �Ի���
class CImageProcessorDlg : public CDialogEx
{
// ����
public:
	CImageProcessorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGEPROCESSOR_DIALOG };
#endif

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
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnReportState(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnFinalResultGet(WPARAM wparam, LPARAM lparam);

private:
	int GetNum(CString & strData, LPCSTR lpszTag);
	void ReportState(CString report);

private:
	ImageReceiver* m_imageReceiver;
	BITMAP* m_finalResult;
};
