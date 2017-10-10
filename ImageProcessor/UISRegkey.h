#pragma once
#include "winreg.h"

class CUISRegKey
{
//Construction
public:
	CUISRegKey();
	virtual ~CUISRegKey();

//Operations
public:
	LONG Open(HKEY hKeyRoot,LPCTSTR pszPath);
	void Close();

	LONG Write(LPCTSTR pszKey,DWORD dwVal);
	LONG Write(LPCTSTR pszKey,LPCTSTR pszVal);
	LONG Write(LPCTSTR pszKey,const BYTE * pData,DWORD dwLength);
	LONG Read(LPCTSTR pszKey,DWORD& dwVal);
	LONG Read(LPCTSTR pszKey,CString& sVal);

	LONG Read(LPCTSTR pszKey,BYTE * pData,DWORD dwlength);
	BOOL DeleteKey(LPCTSTR lpSubKey);
	BOOL DeleteValue(LPCTSTR lpValueString);
protected:
	HKEY m_hKey;
	CString m_strPath;
};

BOOL ReadPathFromRegister
(
	IN LPCTSTR lpszKeyName,			// 注册表的项名称
	OUT CString& strPath			// 模块的ID
);
