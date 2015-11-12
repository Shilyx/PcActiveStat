#include "slxcommon.h"
#include <winternl.h>
#include <Shlwapi.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <ShlObj.h>
#include <list>

#pragma warning(disable: 4996)

using namespace std;

int MessageBoxFormatA(HWND hWindow, LPCSTR lpCaption, UINT uType, LPCSTR lpFormat, ...)
{
    char szBuffer[2048];
    va_list pArg;

    va_start(pArg, lpFormat);
    wvnsprintfA(szBuffer, RTL_NUMBER_OF(szBuffer), lpFormat, pArg);
    va_end(pArg);

    return MessageBoxA(hWindow, szBuffer, lpCaption, uType);
}

int MessageBoxFormatW(HWND hWindow, LPCWSTR lpCaption, UINT uType, LPCWSTR lpFormat, ...)
{
    WCHAR szBuffer[2048];
    va_list pArg;

    va_start(pArg, lpFormat);
    wvnsprintfW(szBuffer, RTL_NUMBER_OF(szBuffer), lpFormat, pArg);
    va_end(pArg);

    return MessageBoxW(hWindow, szBuffer, lpCaption, uType);
}

std::string fmtA(const char *fmt, ...)
{
    char szText[1024 * 10];
    va_list val;

    szText[0] = '\0';
    va_start(val, fmt);
    wvnsprintfA(szText, RTL_NUMBER_OF(szText), fmt, val);
    va_end(val);

    return szText;
}

std::wstring fmtW(const wchar_t *fmt, ...)
{
    wchar_t szText[1024 * 10];
    va_list val;

    szText[0] = L'\0';
    va_start(val, fmt);
    wvnsprintfW(szText, RTL_NUMBER_OF(szText), fmt, val);
    va_end(val);

    return szText;
}

std::string GetNowTimeStringA()
{
    SYSTEMTIME st;

    GetLocalTime(&st);

    return fmtA(TIMEFMT_YMDHMSMA, TIMEEXP_YMDHMSM(st));
}

std::wstring GetNowTimeStringW()
{
    SYSTEMTIME st;

    GetLocalTime(&st);

    return fmtW(TIMEFMT_YMDHMSMW, TIMEEXP_YMDHMSM(st));
}

LPSYSTEMTIME Time1970ToLocalTime(DWORD dwTime1970, LPSYSTEMTIME lpSt)
{
    SYSTEMTIME st1970 = {0};
    FILETIME ft1970;
    FILETIME ftNewTime;

    st1970.wYear = 1970;
    st1970.wMonth = 1;
    st1970.wDay = 1;

    SystemTimeToFileTime(&st1970, &ft1970);
    FileTimeToLocalFileTime(&ft1970, &ftNewTime);
    ((PULARGE_INTEGER)&ftNewTime)->QuadPart += (unsigned __int64)dwTime1970 * 1000 * 1000 * 10;
    FileTimeToSystemTime(&ftNewTime, lpSt);

    return lpSt;
}

DWORD LocalTimeToTime1970(const LPSYSTEMTIME lpSt)
{
    SYSTEMTIME st1970 = {0};
    FILETIME ft1970;
    FILETIME ftNewTime;
    FILETIME ftLocalTime;

    st1970.wYear = 1970;
    st1970.wMonth = 1;
    st1970.wDay = 1;

    SystemTimeToFileTime(&st1970, &ft1970);
    SystemTimeToFileTime(lpSt, &ftNewTime);
    LocalFileTimeToFileTime(&ftNewTime, &ftLocalTime);

    if (CompareFileTime(&ftLocalTime, &ft1970) < 0)
    {
        return 0;
    }

    unsigned __int64 nValue = ((PULARGE_INTEGER)&ftLocalTime)->QuadPart - ((PULARGE_INTEGER)&ft1970)->QuadPart;

    nValue /= (1000 * 1000 * 10);

    if (nValue > 0xffffffff)
    {
        return 0xffffffff;
    }
    else
    {
        return (DWORD)nValue;
    }
}

BOOL ForceDirectoriesA(LPCSTR lpPath)
{
    WCHAR szPath[MAX_PATH];

    wnsprintfW(szPath, RTL_NUMBER_OF(szPath), L"%hs", lpPath);

    return ForceDirectoriesW(szPath);
}

BOOL ForceDirectoriesW(LPCWSTR lpPath)
{
    return 0 == SHCreateDirectory(NULL, lpPath);
}

BOOL ForceContainingDirectoriesA(LPCSTR lpPath)
{
    WCHAR szPath[MAX_PATH];

    wnsprintfW(szPath, RTL_NUMBER_OF(szPath), L"%hs", lpPath);

    return ForceContainingDirectoriesW(szPath);
}

BOOL ForceContainingDirectoriesW(LPCWSTR lpPath)
{
    WCHAR szPath[MAX_PATH];

    lstrcpynW(szPath, lpPath, RTL_NUMBER_OF(szPath));
    PathRemoveFileSpecW(szPath);

    return ForceDirectoriesW(szPath);
}
