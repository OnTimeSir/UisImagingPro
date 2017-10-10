#pragma once
#include <vector>
using namespace std;

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
	int ProcessImage(vector<BITMAP* >& vect, BITMAP& bitmap);
};

