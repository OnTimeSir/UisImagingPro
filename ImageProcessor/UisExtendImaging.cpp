#include "stdafx.h"
#include "UisExtendImaging.h"


CUisExtendImaging::CUisExtendImaging()
{
	//图像拼接参数
	bool m_bTryGpu = false;
	bool m_bWaveCorrect = false;
	double m_dSeamResol = -1;
	float m_fRegisResol = -1;
	float m_fComposResol = -1;
	float m_fAKAZEThre = 0.001f;
	float m_fConfThresh = 1.0f;
	float m_fMatchConf = 0.3f;

	//创建一个stitcher对象
	m_Stitcher = Stitcher::createDefault(m_bTryGpu);
	m_Stitcher.setRegistrationResol(m_fRegisResol);
	m_Stitcher.setCompositingResol(m_fComposResol);

	//特征点检测
	Ptr<detail::FeaturesFinder> finder = new detail::AKAZEFeaturesFinder(5, 0, 3, m_fAKAZEThre);
	//Ptr<detail::FeaturesFinder> finder = new detail::SurfFeaturesFinder(300.0, 3, 4, 3, 4);
	m_Stitcher.setFeaturesFinder(finder);
	//特征点匹配
	Ptr<detail::FeaturesMatcher> matcher = new detail::BestOf2NearestMatcher(m_bTryGpu, m_fMatchConf);
	m_Stitcher.setFeaturesMatcher(matcher);
	//将置信度高于阈值的所有匹配合并到一个集合中
	m_Stitcher.setPanoConfidenceThresh(m_fConfThresh);
	//使用光束平均法进一步精准的估计出旋转矩阵
	Ptr<detail::BundleAdjusterBase> adjuster = new detail::BundleAdjusterRay();
	m_Stitcher.setBundleAdjuster(adjuster);
	//波形矫正
	m_Stitcher.setWaveCorrection(m_bWaveCorrect);
	m_Stitcher.setWaveCorrectKind(detail::WAVE_CORRECT_HORIZ);
	//缝隙估计
	m_Stitcher.setSeamEstimationResol(m_dSeamResol);
	Ptr<detail::SeamFinder> seamFinder = new detail::GraphCutSeamFinder(detail::GraphCutSeamFinderBase::COST_COLOR);
	m_Stitcher.setSeamFinder(seamFinder);
	//光照补偿
	Ptr<detail::ExposureCompensator> compensator = new detail::GainCompensator();
	m_Stitcher.setExposureCompensator(compensator);
	//融合
	Ptr<WarperCreator> warper = new PlaneWarper();
	m_Stitcher.setWarper(warper);
	Ptr<detail::Blender> blender = new detail::MultiBandBlender(m_bTryGpu);
	m_Stitcher.setBlender(blender);
}


CUisExtendImaging::~CUisExtendImaging()
{
	m_InputImage.clear();
}

bool CUisExtendImaging::StitchImage(vector<BITMAP*>& vect, BITMAP& bitmap)
{
	//数据类型转换
	Mat img;
	m_InputImage.clear();
	for (vector <BITMAP*>::iterator  iterp = vect.begin();iterp != vect.end();++iterp)
	{
		Bitmap2Mat(*(*iterp), img);
		m_InputImage.push_back(img);
	}
	//估计变换矩阵
	Stitcher::Status status;
	status = m_Stitcher.estimateTransform(m_InputImage);
	if (status != Stitcher::OK)
	{
		return FALSE;
	}
	//生成全景图像
	status = m_Stitcher.composePanorama(m_PanoImage);
	//转为BITMAP
	Mat2Bitmap(m_PanoImage, bitmap);
	return TRUE;
}

void CUisExtendImaging::Bitmap2Mat(BITMAP & bitmap, Mat & mat)
{
	mat.cols = bitmap.bmWidth;
	mat.rows = bitmap.bmHeight;
	mat.data = (BYTE*)bitmap.bmBits;
	mat.step = bitmap.bmWidthBytes;
}

void CUisExtendImaging::Mat2Bitmap(Mat & mat, BITMAP & bitmap)
{
	bitmap.bmType = 0;
	bitmap.bmWidth = mat.cols;
	bitmap.bmHeight = mat.rows;
	bitmap.bmWidthBytes = mat.cols * 4;
	bitmap.bmPlanes = 1;
	bitmap.bmBitsPixel = 32;
	BYTE * pdata = new BYTE[mat.cols*mat.cols * 4];
	memcpy(pdata, mat.data, mat.cols*mat.cols * 4);
	bitmap.bmBits = pdata;
}
