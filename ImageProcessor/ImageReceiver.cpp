#include "stdafx.h"
#include "ImageProcessor.h"
#include "ImageReceiver.h"
#include "ImageProcessorDlg.h"


ImageReceiver::ImageReceiver(HWND hMainWindow)
	//:m_oEvent1(FALSE, FALSE, (CString(globalDataStruct.strSharedMemoryKey) + _T("Event1")).GetBuffer())
 //   ,m_oEvent2(FALSE, FALSE, (CString(globalDataStruct.strSharedMemoryKey) + _T("Event1")).GetBuffer())
	: m_oEvent1(FALSE, FALSE, _T("UIS4DFrame1ValidEvent"))
	, m_oEvent2(FALSE, FALSE, _T("UIS4DFrame2ValidEvent"))
	,m_hRecvThread1(NULL)
	,m_hRecvThread2(NULL)
	,m_hImageProcessThread(NULL)
	,m_dequeLocker(0,"DequeLocker",0)
{
	m_hMainWindow = hMainWindow;	
}


ImageReceiver::~ImageReceiver()
{
	stop();
}

void ImageReceiver::startReceive()
{
	PostMessage(m_hMainWindow,WM_REPORT_STATE,0, (LPARAM)(new CString("Start to receive image...")));
	
	//打开共享内存
	m_hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (LPCSTR)globalDataStruct.strSharedMemoryKey);
	if (m_hFileMapping == NULL)
	{
		PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("No shared memory found by key '" + globalDataStruct.strSharedMemoryKey + "'")));
		return;
	}
	else 
	{
		PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("Shared memory found by key '" + globalDataStruct.strSharedMemoryKey + "'")));
	}

	m_pBuf1 = (char *)MapViewOfFile(m_hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	m_pBuf2 = m_pBuf1 + globalDataStruct.nBufSize;

	if (m_pBuf1 == NULL || m_pBuf2 == NULL)
	{
		PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("Start address of  MapViewOfFile is NULL...")));
		return;
	}
	PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("Start address of  MapViewOfFile is geted...")));

	m_hRecvThread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Buf1RecvThread, (LPVOID)this, 0, 0);
	m_hRecvThread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Buf2RecvThread, (LPVOID)this, 0, 0);
	m_hImageProcessThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ImageProcessThread, (LPVOID)this, 0, 0);
	SetThreadPriority(m_hRecvThread1, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(m_hRecvThread2, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(m_hImageProcessThread, THREAD_PRIORITY_NORMAL);
}

void ImageReceiver::stop()
{
	PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("To stop all work thread...")));
	if (m_hRecvThread1)
	{
		TerminateThread(m_hRecvThread1, 0);
		CloseHandle(m_hRecvThread1);
		WaitForSingleObject(m_hRecvThread1, INFINITE);
		m_hRecvThread1 = 0;
		PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("Buf1RecvThread is stoped...")));
	}

	if (m_hRecvThread2)
	{
		TerminateThread(m_hRecvThread2, 0);
		CloseHandle(m_hRecvThread2);
		WaitForSingleObject(m_hRecvThread2, INFINITE);
		m_hRecvThread2 = 0;
		PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("Buf2RecvThread is stoped...")));
	}

	if (m_hImageProcessThread)
	{
		TerminateThread(m_hImageProcessThread, 0);
		CloseHandle(m_hImageProcessThread);
		WaitForSingleObject(m_hImageProcessThread, INFINITE);
		m_hImageProcessThread = 0;
		PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("ImageProcessThread is stoped...")));
	}

	m_dequeLocker.Lock();
	deque<BITMAP*>::iterator iter;
	iter = m_qRecvdData.begin();
	while (iter != m_qRecvdData.end()) {
		delete ((BITMAP*)(*iter))->bmBits;
		delete  *iter;                    //先用迭代器释放空间，再用erase
		iter = m_qRecvdData.erase(iter);
	}
	m_qRecvdData.swap(deque<BITMAP*>());
	m_dequeLocker.Unlock();

	vector <BITMAP*>::iterator iterp;     //定义迭代器
	iterp = m_imgs.begin();
	while (iterp != m_imgs.end())
	{
		delete ((BITMAP*)(*iterp))->bmBits;
		delete  *iterp;                   //先用迭代器释放空间，再用erase
		iterp = m_imgs.erase(iterp);
	}
	m_imgs.swap(vector <BITMAP*>());

	if (m_pBuf1)
	{
		UnmapViewOfFile(m_pBuf1);
		m_pBuf1 = NULL;
	}
	if (m_pBuf2)
	{
		UnmapViewOfFile(m_pBuf2);
		m_pBuf2 = NULL;
	}
	PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("all buff is cleared...")));

	if (m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = 0;
	}
	PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("FileMapping handle is closed...")));
}

UINT ImageReceiver::Buf1RecvThread(LPVOID lpParameter)
{
	ImageReceiver * pThis = (ImageReceiver*)lpParameter;
	PostMessage(pThis->m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("Buf1RecvThread is start to receive image...")));
	return pThis->DoHandleBufRecv(pThis->m_pBuf1, pThis->m_oEvent1.m_hObject, lpParameter);
}

UINT ImageReceiver::Buf2RecvThread(LPVOID lpParameter)
{
	ImageReceiver * pThis = (ImageReceiver*)lpParameter;
	PostMessage(pThis->m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("Buf2RecvThread is start to receive image...")));
	return pThis->DoHandleBufRecv(pThis->m_pBuf2, pThis->m_oEvent2.m_hObject, lpParameter);
}

UINT ImageReceiver::ImageProcessThread(LPVOID lpParameter)
{
	ImageReceiver * pThis = (ImageReceiver*)lpParameter;
	PostMessage(pThis->m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("ImageProcessThread is start to process image...")));
	return pThis->DoHandleImageProcess(lpParameter);
}

UINT ImageReceiver::DoHandleBufRecv(void * pBuf, HANDLE hEvent, LPVOID lpParameter)
{
	ImageReceiver * pThis = (ImageReceiver*)lpParameter;
	while (1)
	{
		WaitForSingleObject(hEvent, INFINITE);
		
		SetFrame(pBuf);
	}
	return 0;
}

BOOL ImageReceiver::SetFrame(void * pBuf)
{
	BYTE * pFrameData = new BYTE[globalDataStruct.nBufSize];
	memcpy(pFrameData, pBuf, globalDataStruct.nBufSize);
	BITMAP* pBitmap = new BITMAP();
	pBitmap->bmType = 0;
	pBitmap->bmWidth = globalDataStruct.rectImageArea.Width();
	pBitmap->bmHeight = globalDataStruct.rectImageArea.Height();
	pBitmap->bmWidthBytes = globalDataStruct.rectImageArea.Width() * 4;
	pBitmap->bmPlanes = 1;
	pBitmap->bmBitsPixel = 32;
	pBitmap->bmBits = pFrameData;
	PostMessage(m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("a frame of image received from shared memory...")));
	m_dequeLocker.Lock();
	m_qRecvdData.push_front(pBitmap);
	m_dequeLocker.Unlock();
	return TRUE;
}

UINT ImageReceiver::DoHandleImageProcess(LPVOID lpParameter)
{
	BITMAP* pBitmap = NULL;
	ImageReceiver * pThis = (ImageReceiver*)lpParameter;
	while (1) {
		m_dequeLocker.Lock();
		if (0 != m_qRecvdData.size() && m_imgs.size() < MAX_IMAGES_PER_PROCESS) {
			pBitmap = m_qRecvdData.back();
			m_qRecvdData.pop_back();
			if (NULL != pBitmap) {
				m_imgs.push_back(pBitmap);
			}
		} 
		m_dequeLocker.Unlock();

		if (MAX_IMAGES_PER_PROCESS == m_imgs.size()) { //达到队列最大值，开始本次数据处理
			BITMAP* bitmap = new BITMAP();
			int result = m_uisExtendImaging.ProcessImage(m_imgs,*bitmap);
			if (-1 == result) {       //表示拼接出现错误结果，需要终止此次图片处理过程
				PostMessage(pThis->m_hMainWindow, WM_FINAL_RESULT, -1, (LPARAM)(new CString("ImageProcessThread：a error-result returned by Func 'ProcessImage'")));
			}else if (0 == result) {  //表示拼接后的图片是最终结果，bitmap保存此次拼接的最终结果，通知主界面接收并显示结果，并关闭接收与处理线程
				PostMessage(pThis->m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("ImageProcessThread：a final result returned by Func 'ProcessImage', Yeah!!")));			
				SendMessage(pThis->m_hMainWindow, WM_FINAL_RESULT, 0, (LPARAM)(bitmap));
			}else if (1 == result) {  //表示拼接后的图片不是最终结果，bitmap保存此次拼接的中间结果
				PostMessage(pThis->m_hMainWindow, WM_REPORT_STATE, 0, (LPARAM)(new CString("ImageProcessThread：a inter-result returned by Func 'ProcessImage'")));
				if (NULL != bitmap) {
					//清空vector，释放图片空间
					vector <BITMAP*>::iterator  iterp;     //定义迭代器
					iterp = m_imgs.begin();
					while (iterp != m_imgs.end())
					{
						delete ((BITMAP*)(*iterp))->bmBits;
						delete  *iterp;                   //先用迭代器释放空间，再用erase
						iterp = m_imgs.erase(iterp);
					}
					m_imgs.swap(vector <BITMAP*>());
					m_imgs.push_back(bitmap);    //将中间结果塞回vector
				}
			}else {                   //非正常返回值,按出错处理
				PostMessage(pThis->m_hMainWindow, WM_REPORT_STATE, -1, (LPARAM)(new CString("ImageProcessThread：a unexpected-result(Not -1 0 1) returned by Func 'ProcessImage'")));
			}

		}
	}
}
