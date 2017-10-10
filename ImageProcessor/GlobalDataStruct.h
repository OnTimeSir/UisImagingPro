#pragma once

using namespace std;

class GlobalDataStruct;
static class GlobalDataStruct* intance;

class GlobalDataStruct
{
public:
	GlobalDataStruct();
	~GlobalDataStruct();

	CRect rectImageArea;
	size_t nBufSize;
	CString strSharedMemoryKey;
};

extern GlobalDataStruct globalDataStruct;

