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

	
	/* 入参：
	*  vect保存需要处理的图片
	*  bit 保存处理结果图片
	*  返回值:
	*  0 -- 表示已完成最后的拼接，bitmap保存最终结果；
	*  1 -- 表示拼接后的图片不是最终结果，bitmap保存此次拼接的中间结果
	*  -1 -- 表示拼接出现错误结果，需要终止此次图片处理过程
	*/
	bool StitchImage(vector<BITMAP* >& vect, BITMAP& bitmap);
private:
	void SetParameters();
	void SetPipeline();
	void Bitmap2Mat(BITMAP& bitmap, Mat& mat);
	void Mat2Bitmap(Mat& mat, BITMAP& bitmap);
	//输入图片序列
	vector<Mat> m_InputImage;
	//拼接结果图片
	Mat m_PanoImage;
	//图像拼接参数
	bool m_bTryGpu;
	bool m_bWaveCorrect;
	double m_dSeamResol;
	float m_fRegisResol;
	float m_fComposResol;
	float m_fAKAZEThre; //AKAZE特征检测阈值
	float m_fConfThresh;//在同一全景图内的置信阈值
	float m_fMatchConf;//特征点检测置信等级，最近邻匹配距离与次近邻匹配距离的比值

					   //stitcher对象
	Stitcher m_Stitcher;
};

