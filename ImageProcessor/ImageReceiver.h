#pragma once
#include <deque>
#include <afxmt.h>
#include <vector>
#include "UisExtendImaging.h"

using namespace std;

class ImageReceiver
{
public:
	ImageReceiver(HWND hMainWindow);
	virtual ~ImageReceiver(void);

	void startReceive();
	void stop();
private:
	HWND m_hMainWindow;         //�����ڽ���
	HANDLE m_hFileMapping;      //�����ڴ��ļ�ӳ����
	char * m_pBuf1;             //�����ڴ�ĵ�һ��ͼƬ�����׵�ַ
	char * m_pBuf2;             //�����ڴ�ĵڶ���ͼƬ�����׵�ַ

	CEvent m_oEvent1;           //ʵ�ֽ��̼乲���ڴ���ʵ��������ʵ�һ��ͼƬ��
	CEvent m_oEvent2;           //ʵ�ֽ��̼乲���ڴ���ʵ��������ʵڶ���ͼƬ��

	deque<BITMAP*> m_qRecvdData;  //��Ŵӹ����ڴ���ȡ�õ�����
	vector<BITMAP*> m_imgs;       //��Ÿ�������������
	BITMAP* m_finalResult;        //���յĴ�����

	CMutex m_dequeLocker;         //���ʶ��е���
	
	HANDLE m_hRecvThread1;
	HANDLE m_hRecvThread2;
	HANDLE m_hImageProcessThread;

	float m_fResaultSize;     //���ս��ͼ�Ŀ�߱�
	int m_nImagesPerProcess; //һ�δ����֡��
	CUisExtendImaging m_uisExtendImaging;

private:
	static UINT Buf1RecvThread(LPVOID lpParameter);
	static UINT Buf2RecvThread(LPVOID lpParameter);
	static UINT ImageProcessThread(LPVOID lpParameter);

	UINT DoHandleBufRecv(void * pBuf, HANDLE hEvent, LPVOID lpParameter);
	BOOL SetFrame(void * pBuf);
	UINT DoHandleImageProcess(LPVOID lpParameter);
};

