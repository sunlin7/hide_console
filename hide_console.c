/* Run process without console window (just for windows). */

#include <Windows.h>
#include <tchar.h>

void ErrorExit(LPCTSTR lpszFunction, LPCTSTR lpMsg)
{
	TCHAR szBuf[80];
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	wsprintf(szBuf,
		_T("%s failed with error %d: %s(MSG:%s)"),
		lpszFunction, dw, lpMsgBuf, lpMsg);

}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR     lpCmdLine,
	int       nCmdShow)
{
	LPTSTR              pCmd         = lpCmdLine;
	BOOL                bOptWait     = FALSE;
	LPCTSTR             strOptWait   = _T("-w ");
	LPCTSTR             strOptVerify = _T("-v ");
	STARTUPINFO         si           = {0};
	PROCESS_INFORMATION pi           = {0};
	si.cb = sizeof(si);
	GetStartupInfo(&si);
	si.wShowWindow = SW_HIDE;
	if (NULL != pCmd && *pCmd ) {
		if (0 == _tcsncicmp(pCmd, strOptWait, _tcslen(strOptWait))) {
			bOptWait = TRUE;
			pCmd += _tcslen(strOptWait);
			for ( ; _T(' ') == *pCmd; pCmd++) // skip the space
				;
		}
		if ( 0 == _tcsncicmp(pCmd, strOptVerify, _tcslen(strOptVerify))) {
			si.wShowWindow = SW_SHOWNORMAL;
			for ( ; _T(' ') == *pCmd; pCmd++)
				;
		}
		// Start the child process.
		if (!CreateProcess(NULL, pCmd, NULL, NULL, FALSE,
			CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		{
			ErrorExit(_T("CreateProcess"), pCmd);
			return 0;
		}
		if (bOptWait) {
			// Wait until child process exits.
			WaitForSingleObject( pi.hProcess, INFINITE);
		}
		// Close process and thread handles.
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	} else {
		MessageBox(NULL, _T("Usage: HideConsole command with arguments."), _T("Usage"), MB_OK);
	}
	return 0;
}

#ifdef NDEBUG

#define SPACECHAR _T(' ')
#define DQUOTECHAR _T('\"')

TCHAR * wincmdln(void)
{
	TCHAR *lpszCommandLine = GetCommandLine();
	if (*lpszCommandLine == DQUOTECHAR) {
		while ( (*(++lpszCommandLine) != DQUOTECHAR)
			&& (*lpszCommandLine != _T('\0')) )
		{
#ifdef _MBCS
			if (_ismbblead(*lpszCommandLine))
				lpszCommandLine++;
#endif
		}
		if (*lpszCommandLine == DQUOTECHAR)
			lpszCommandLine++;
	} else {
		while (*lpszCommandLine > SPACECHAR)
			lpszCommandLine++;
	}

	while (*lpszCommandLine && (*lpszCommandLine <= SPACECHAR))
		lpszCommandLine++;
	return lpszCommandLine;
}

int startup(void) 
{
	return _tWinMain(NULL, NULL, wincmdln(), SW_SHOWNORMAL);
}

#endif
