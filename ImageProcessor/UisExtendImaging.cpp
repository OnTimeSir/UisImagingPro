#include "stdafx.h"
#include "UisExtendImaging.h"


CUisExtendImaging::CUisExtendImaging()
{
	//����һ��stitcher����
	m_Stitcher = Stitcher::createDefault(m_bTryGpu);
	SetParameters();
	SetPipeline();
}


CUisExtendImaging::~CUisExtendImaging()
{
	m_InputImage.clear();
}

bool CUisExtendImaging::StitchImage(vector<BITMAP*>& vect, BITMAP& bitmap)
{
	//��������ת��
	Mat img;
	m_InputImage.clear();
	img = imread("D:\\1.bmp");
	m_InputImage.push_back(img);
	img = imread("D:\\2.bmp");
	m_InputImage.push_back(img);
	img = imread("D:\\3.bmp");
	m_InputImage.push_back(img);
	img = imread("D:\\4.bmp");
	m_InputImage.push_back(img);
	//for (vector <BITMAP*>::iterator  iterp = vect.begin();iterp != vect.end();++iterp)
	//{
	//	Bitmap2Mat(*(*iterp), img);
	//	m_InputImage.push_back(img);
	//}
	//���Ʊ任����
	Stitcher::Status status;
	//status = m_Stitcher.estimateTransform(m_InputImage);
	status = m_Stitcher.stitch(m_InputImage, m_PanoImage);
	if (status != Stitcher::OK)
	{
		return FALSE;
	}
	//����ȫ��ͼ��
	//status = m_Stitcher.composePanorama(m_PanoImage);
	imwrite("D:\\result.bmp", m_PanoImage);
	//תΪBITMAP
	Mat2Bitmap(m_PanoImage, bitmap);
	return TRUE;
}

void CUisExtendImaging::Bitmap2Mat(BITMAP & bitmap, Mat & mat)
{
	mat = Mat(bitmap.bmHeight, bitmap.bmWidth, CV_8UC3);
	mat.data = (BYTE*)bitmap.bmBits;
	mat.step = bitmap.bmWidthBytes;
}

void CUisExtendImaging::Mat2Bitmap(Mat & mat, BITMAP & bitmap)
{
	bitmap.bmType = 0;
	bitmap.bmWidth = mat.cols;
	bitmap.bmHeight = mat.rows;
	bitmap.bmWidthBytes = mat.cols * 3;
	bitmap.bmPlanes = 1;
	bitmap.bmBitsPixel = 24;
	bitmap.bmBits = mat.data;
}

void CUisExtendImaging::SetParameters()
{
	//ͼ��ƴ�Ӳ���
	m_bTryGpu = true;
	m_bWaveCorrect = false;
	m_dSeamResol = -1;
	m_fRegisResol = -1;
	m_fComposResol = -1;
	m_fAKAZEThre = 0.001f;
	m_fConfThresh = 1.0f;
	m_fMatchConf = 0.3f;
}

void CUisExtendImaging::SetPipeline()
{
	m_Stitcher.setRegistrationResol(m_fRegisResol);
	m_Stitcher.setCompositingResol(m_fComposResol);

	//��������
	Ptr<detail::FeaturesFinder> finder = new detail::AKAZEFeaturesFinder(5, 0, 3, m_fAKAZEThre);
	//Ptr<detail::FeaturesFinder> finder = new detail::SurfFeaturesFinder(300.0, 3, 4, 3, 4);
	m_Stitcher.setFeaturesFinder(finder);
	//������ƥ��
	Ptr<detail::FeaturesMatcher> matcher = new detail::BestOf2NearestMatcher(m_bTryGpu, m_fMatchConf);
	m_Stitcher.setFeaturesMatcher(matcher);
	//�����Ŷȸ�����ֵ������ƥ��ϲ���һ��������
	m_Stitcher.setPanoConfidenceThresh(m_fConfThresh);
	//ʹ�ù���ƽ������һ����׼�Ĺ��Ƴ���ת����
	Ptr<detail::BundleAdjusterBase> adjuster = new detail::BundleAdjusterRay();
	m_Stitcher.setBundleAdjuster(adjuster);
	//���ν���
	m_Stitcher.setWaveCorrection(m_bWaveCorrect);
	m_Stitcher.setWaveCorrectKind(detail::WAVE_CORRECT_HORIZ);
	//��϶����
	m_Stitcher.setSeamEstimationResol(m_dSeamResol);
	Ptr<detail::SeamFinder> seamFinder = new detail::GraphCutSeamFinder(detail::GraphCutSeamFinderBase::COST_COLOR);
	m_Stitcher.setSeamFinder(seamFinder);
	//���ղ���
	Ptr<detail::ExposureCompensator> compensator = new detail::GainCompensator();
	m_Stitcher.setExposureCompensator(compensator);
	//�ں�
	Ptr<WarperCreator> warper = new PlaneWarper();
	m_Stitcher.setWarper(warper);
	Ptr<detail::Blender> blender = new detail::MultiBandBlender(m_bTryGpu);
	m_Stitcher.setBlender(blender);
}
