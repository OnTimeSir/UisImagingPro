#pragma once
#include <vector>
using namespace std;

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
	int ProcessImage(vector<BITMAP* >& vect, BITMAP& bitmap);
};

