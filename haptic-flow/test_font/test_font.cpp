#include <string>
#include <iostream>

/*
int truc(string& plouf)
{
	return 0;
}

BOOL GetFontFile(LPCTSTR lpszFontName, 
                 string& strDisplayName, 
                 string& strFontFile)
{

	TCHAR szName[2 * MAX_PATH];
	TCHAR szData[2 * MAX_PATH];

	LPCTSTR strFont = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";

	BOOL bResult = FALSE;

	while (GetNextNameValue(HKEY_LOCAL_MACHINE, strFont, szName, szData) == 
         ERROR_SUCCESS)
	{
		if (_strnicmp(lpszFontName, szName, strlen(lpszFontName)) == 0)
		{
			strDisplayName = szName;
			strFontFile = szData;
			bResult = TRUE;
			break;
		}
    strFont = "";


	}
  // close the registry key
	GetNextNameValue(HKEY_LOCAL_MACHINE, NULL, NULL, NULL);	

	return bResult;
}
*/

int main(void)
{

	return 0;

}
