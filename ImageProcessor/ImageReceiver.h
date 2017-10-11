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
	HWND m_hMainWindow;         //主窗口界面
	HANDLE m_hFileMapping;      //共享内存文件映射句柄
	char * m_pBuf1;             //共享内存的第一张图片区的首地址
	char * m_pBuf2;             //共享内存的第二张图片区的首地址

	CEvent m_oEvent1;           //实现进程间共享内存访问的锁（访问第一张图片）
	CEvent m_oEvent2;           //实现进程间共享内存访问的锁（访问第二张图片）

	deque<BITMAP*> m_qRecvdData;  //存放从共享内存中取得的数据
	vector<BITMAP*> m_imgs;       //存放给处理函数的向量
	BITMAP* m_finalResult;        //最终的处理结果

	CMutex m_dequeLocker;         //访问队列的锁
	
	HANDLE m_hRecvThread1;
	HANDLE m_hRecvThread2;
	HANDLE m_hImageProcessThread;

	float m_fResaultSize;     //最终结果图的宽高比
	int m_nImagesPerProcess; //一次处理的帧数
	CUisExtendImaging m_uisExtendImaging;

private:
	static UINT Buf1RecvThread(LPVOID lpParameter);
	static UINT Buf2RecvThread(LPVOID lpParameter);
	static UINT ImageProcessThread(LPVOID lpParameter);

	UINT DoHandleBufRecv(void * pBuf, HANDLE hEvent, LPVOID lpParameter);
	BOOL SetFrame(void * pBuf);
	UINT DoHandleImageProcess(LPVOID lpParameter);
};

