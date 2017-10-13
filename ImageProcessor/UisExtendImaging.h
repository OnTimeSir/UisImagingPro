#pragma once
#include <vector>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/stitching.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <Windows.h>
using namespace std;
using namespace cv;
using namespace xfeatures2d;

class CUisExtendImaging
{
public:
	CUisExtendImaging();
	~CUisExtendImaging();

	
	/* ��Σ�
	*  vect������Ҫ�����ͼƬ
	*  bit ���洦����ͼƬ
	*  ����ֵ:
	*  0 -- ��ʾ���������ƴ�ӣ�bitmap�������ս����
	*  1 -- ��ʾƴ�Ӻ��ͼƬ�������ս����bitmap����˴�ƴ�ӵ��м���
	*  -1 -- ��ʾƴ�ӳ��ִ���������Ҫ��ֹ�˴�ͼƬ�������
	*/
	bool StitchImage(vector<BITMAP* >& vect, BITMAP& bitmap);
private:
	void SetParameters();
	void SetPipeline();
	void Bitmap2Mat(BITMAP& bitmap, Mat& mat);
	void Mat2Bitmap(Mat& mat, BITMAP& bitmap);
	//����ͼƬ����
	vector<Mat> m_InputImage;
	//ƴ�ӽ��ͼƬ
	Mat m_PanoImage;
	//ͼ��ƴ�Ӳ���
	bool m_bTryGpu;
	bool m_bWaveCorrect;
	double m_dSeamResol;
	float m_fRegisResol;
	float m_fComposResol;
	float m_fAKAZEThre; //AKAZE���������ֵ
	float m_fConfThresh;//��ͬһȫ��ͼ�ڵ�������ֵ
	float m_fMatchConf;//�����������ŵȼ��������ƥ�������ν���ƥ�����ı�ֵ

					   //stitcher����
	Stitcher m_Stitcher;
};

