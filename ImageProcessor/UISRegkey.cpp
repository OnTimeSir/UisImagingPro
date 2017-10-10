#include "stdafx.h"
#include "UISregkey.h"

CUISRegKey::CUISRegKey()
{
	m_hKey = NULL;
}
CUISRegKey::~CUISRegKey()
{
	Close();
}

LONG CUISRegKey::Open(HKEY hKeyRoot,LPCTSTR pszPath)
{
	DWORD dw;
	m_strPath = pszPath;

	return RegCreateKeyEx(hKeyRoot,pszPath,0L,NULL,
		REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&m_hKey,&dw);
}

void CUISRegKey::Close()
{
	if (m_hKey)
	{
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

LONG CUISRegKey::Write(LPCTSTR pszKey,DWORD dwVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	return RegSetValueEx(m_hKey,pszKey,0L,REG_DWORD,
		(CONST BYTE *)&dwVal,sizeof(DWORD));
}

LONG CUISRegKey::Write(LPCTSTR pszKey,LPCTSTR pszData)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	ASSERT(pszData);
	ASSERT(AfxIsValidAddress(pszData,_tcslen(pszData),FALSE));

	return RegSetValueEx(m_hKey,pszKey,0L,REG_SZ,
		(CONST BYTE *)pszData,(DWORD)_tcslen(pszData) + 1);
}

LONG CUISRegKey::Write(LPCTSTR pszKey,const BYTE * pData,DWORD dwLength)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	ASSERT(pData && dwLength > 0);
	ASSERT(AfxIsValidAddress(pData,dwLength,FALSE));

	return RegSetValueEx(m_hKey,pszKey,0L,REG_BINARY,
		pData,dwLength);
}

LONG CUISRegKey::Read(LPCTSTR pszKey,DWORD& dwVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	
	DWORD dwType;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwDest;

	LONG lRet = RegQueryValueEx(m_hKey,/*(LPSTR)*/pszKey,NULL,&dwType,(BYTE *)&dwDest,&dwSize);
	if (lRet == ERROR_SUCCESS)
		dwVal = dwDest;
	return lRet;
}

LONG CUISRegKey::Read(LPCTSTR pszKey,CString &sVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);

	DWORD dwType;
	DWORD dwSize = 200;
	TCHAR szString[201];

	LONG lRet = RegQueryValueEx(m_hKey,/*(LPSTR)*/pszKey,NULL,&dwType,(BYTE *)szString,&dwSize);
	if (lRet == ERROR_SUCCESS)
		sVal = szString;
	return lRet;
}

LONG CUISRegKey::Read(LPCTSTR pszKey,BYTE * pData,DWORD dwLen)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	
	DWORD dwType;

	LONG lRet = RegQueryValueEx(m_hKey,/*(LPSTR)*/pszKey,NULL,
		&dwType,pData,&dwLen);
	return lRet;
}

BOOL CUISRegKey::DeleteKey(LPCTSTR lpSubKey) 
{
	ASSERT(m_hKey);
	ASSERT(lpSubKey);

	return (! RegDeleteKey(m_hKey,lpSubKey));
}

BOOL CUISRegKey::DeleteValue(LPCTSTR lpValueString) 
{
	ASSERT(m_hKey);
//	ASSERT(lpSubKey);

	return (! RegDeleteValue(m_hKey,lpValueString));
}

BOOL ReadPathFromRegister
(
	IN LPCTSTR lpszKeyName,			// 注册表的项名称
	OUT CString& strPath			// 模块的ID
)
{
	CUISRegKey oRegKey;
	if (oRegKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\UnitedImaging")) != FALSE)
	{
		return FALSE;
	}

	if (oRegKey.Read(lpszKeyName, strPath) != 0)
	{
		return FALSE;
	}

	return TRUE;
}