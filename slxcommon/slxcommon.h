#ifndef _SLXCOMMON_H
#define _SLXCOMMON_H

#ifdef _MSC_VER

#include <Windows.h>

#else /* _MSC_VER */

#endif /* _MSC_VER */

#ifdef __cplusplus

#include <string>
#include <set>
#include "SetOperators.h"

#ifdef _MSC_VER

#include "RegistryOperation.h"
#include "slxpredefines.h"

#ifdef _UNICODE
    #define MessageBoxFormat MessageBoxFormatW
    #define fmt fmtW
    #define GetNowTimeString GetNowTimeStringW
    #define ForceDirectories ForceDirectoriesW
    #define ForceContainingDirectories ForceContainingDirectoriesW
#else
    #define MessageBoxFormat MessageBoxFormatA
    #define fmt fmtA
    #define GetNowTimeString GetNowTimeStringA
    #define ForceDirectories ForceDirectoriesA
    #define ForceContainingDirectories ForceContainingDirectoriesA
#endif

int MessageBoxFormatA(HWND hWindow, LPCSTR lpCaption, UINT uType, LPCSTR lpFormat, ...);
int MessageBoxFormatW(HWND hWindow, LPCWSTR lpCaption, UINT uType, LPCWSTR lpFormat, ...);

std::string fmtA(const char *fmt, ...);
std::wstring fmtW(const wchar_t *fmt, ...);

std::string GetNowTimeStringA();
std::wstring GetNowTimeStringW();

LPSYSTEMTIME Time1970ToLocalTime(DWORD dwTime1970, LPSYSTEMTIME lpSt);
DWORD LocalTimeToTime1970(const LPSYSTEMTIME lpSt);

BOOL ForceDirectoriesA(LPCSTR lpPath);
BOOL ForceDirectoriesW(LPCWSTR lpPath);

BOOL ForceContainingDirectoriesA(LPCSTR lpPath);
BOOL ForceContainingDirectoriesW(LPCWSTR lpPath);

#else /* _MSC_VER */

#endif /* _MSC_VER */

#endif /* __cplusplus */

#endif /* _SLXCOMMON_H */