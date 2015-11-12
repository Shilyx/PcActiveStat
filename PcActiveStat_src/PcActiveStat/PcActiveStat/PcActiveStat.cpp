/*
 *  @file  : PcActiveStat.cpp
 *  @author: luteng
 *  @date  : 2015-09-05 16:04:29.016
 *  @note  : Generated by SlxTemplates
 */

#include <slxcommon.h>
#include <Windows.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include "ExportExcel.h"
#pragma warning(disable: 4786)
#include <tchar.h>
#include "resource.h"
#include <vector>
#include <map>
#include <set>
#include <sstream>

using namespace std;

class CActiveTimeReader
{
public:
    CActiveTimeReader()
    {
        ReadTimesFromSystem();
    }

    ~CActiveTimeReader()
    {
    }

private:
    BOOL ReadTimesFromSystem()
    {
        m_vectorOnTimes.clear();
        m_vectorOffTimes.clear();

        BOOL bSucceed = FALSE;
        HANDLE hEventLog = OpenEventLog(NULL, TEXT("System"));

        if (hEventLog != NULL)
        {
            DWORD dwRecordNumber = 0;
            char szBuffer[sizeof(EVENTLOGRECORD) + 11111];
            DWORD dwBytesRead = 0;
            DWORD dwBytesNeeded = 0;

            if (GetOldestEventLogRecord(hEventLog, &dwRecordNumber))
            {
                bSucceed = TRUE;

                while (ReadEventLog(
                    hEventLog,                                          // Event log handle
                    EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ,  // Reads forward, Sequential read
                    0,                                                  // Ignored for sequential read
                    szBuffer,                                           // Pointer to buffer
                    sizeof(szBuffer),                                   // Size of buffer
                    &dwBytesRead,                                       // Number of bytes read
                    &dwBytesNeeded))                                    // Bytes in the next record
                {
                    LPCSTR lpBuffer = szBuffer;
                    DWORD dwOffset = 0;

                    while (dwOffset < dwBytesRead)
                    {
                        EVENTLOGRECORD *pRecord = (EVENTLOGRECORD *)(lpBuffer + dwOffset);
                        DWORD dwEventId = pRecord->EventID & 0xffff;

                        if (dwEventId == 6005)
                        {
                            m_vectorOnTimes.push_back(pRecord->TimeGenerated);
                        }
                        else if (dwEventId == 6006)
                        {
                            m_vectorOffTimes.push_back(pRecord->TimeGenerated);
                        }
                        else
                        {
                        }

                        dwOffset += pRecord->Length;
                    }
                }
            }

            CloseEventLog(hEventLog);
        }

        return bSucceed;
    }

    static BOOL SaveTimePairs(int nYear, int nMonth, const map<DWORD, DWORD> &mapTimePairs)
    {
        WCHAR szRegPath[1024];

        wnsprintfW(szRegPath, RTL_NUMBER_OF(szRegPath), L"Software\\Shilyx Studio\\PcActiveStat\\Cache\\%04u\\%02u", nYear, nMonth);

        if (!mapTimePairs.empty())
        {
            vector<DWORD> vectorTimes;

            vectorTimes.reserve(mapTimePairs.size() * 2);

            for (map<DWORD, DWORD>::const_iterator it = mapTimePairs.begin(); it != mapTimePairs.end(); ++it)
            {
                vectorTimes.push_back(it->first);
                vectorTimes.push_back(it->second);
            }

            SHSetValueW(HKEY_CURRENT_USER, szRegPath, L"data", REG_BINARY, &*vectorTimes.begin(), (DWORD)vectorTimes.size() * sizeof(DWORD));
        }
        else
        {
            SHDeleteValueW(HKEY_CURRENT_USER, szRegPath, L"data");
        }

        wstring strNow = GetNowTimeStringW();
        SHSetValueW(HKEY_CURRENT_USER, szRegPath, L"update time", REG_SZ, strNow.c_str(), ((DWORD)strNow.size() + 1) * sizeof(WCHAR));

        return TRUE;
    }

    static map<DWORD, DWORD> LoadTimePairs(int nYear, int nMonth)
    {
        map<DWORD, DWORD> mapResult;
        WCHAR szRegPath[1024];
        DWORD dwTimePairs[8192];
        DWORD dwRegType = REG_NONE;
        DWORD dwSize = sizeof(dwTimePairs);

        wnsprintfW(szRegPath, RTL_NUMBER_OF(szRegPath), L"Software\\Shilyx Studio\\PcActiveStat\\Cache\\%04u\\%02u", nYear, nMonth);

        SHGetValueW(HKEY_CURRENT_USER, szRegPath, L"data", &dwRegType, dwTimePairs, &dwSize);

        if (dwRegType == REG_BINARY && dwSize > 0)
        {
            dwSize /= sizeof(DWORD) * 2;
        }
        else
        {
            dwSize = 0;
        }

        for (DWORD dwIndex = 0; dwIndex < dwSize; ++dwIndex)
        {
            mapResult[dwTimePairs[dwIndex * 2]] = dwTimePairs[dwIndex * 2 + 1];
        }

        return mapResult;
    }

public:
    map<DWORD, DWORD> GetYearMonthTimePairs(int nYear, int nMonth) const
    {
        map<DWORD, DWORD> mapResult = LoadTimePairs(nYear, nMonth);
        map<DWORD, bool> mapAllTimes;
        SYSTEMTIME st;

        // 加入所有开机时间
        for (vector<DWORD>::const_iterator it = m_vectorOnTimes.begin(); it != m_vectorOnTimes.end(); ++it)
        {
            Time1970ToLocalTime(*it, &st);

            if (st.wYear == nYear && st.wMonth == nMonth)
            {
                mapAllTimes[*it] = true;
            }
        }

        // 加入所有关机时间
        for (vector<DWORD>::const_iterator it = m_vectorOffTimes.begin(); it != m_vectorOffTimes.end(); ++it)
        {
            Time1970ToLocalTime(*it, &st);

            if (st.wYear == nYear && st.wMonth == nMonth)
            {
                mapAllTimes[*it] = false;
            }
        }

        if (!mapAllTimes.empty())
        {
            // 增加伪开机时间难以兼容事件记录不全的情形
//             // 增加一个最开始的伪开机时间，月初，如果已存在缓存则忽略
//             if (mapResult.empty())
//             {
//                 SYSTEMTIME st;
// 
//                 ZeroMemory(&st, sizeof(st));
//                 st.wYear = nYear;
//                 st.wMonth = nMonth;
//                 st.wDay = 1;
// 
//                 mapAllTimes[LocalTimeToTime1970(&st)] = true;
//             }

            // 增加一个最开始的伪关机时间，月末，但不晚于当前时间
            {
                SYSTEMTIME stNow;
                SYSTEMTIME st;

                GetLocalTime(&stNow);

                ZeroMemory(&st, sizeof(st));
                st.wYear = nYear;
                st.wMonth = nMonth + 1;
                st.wDay = 1;

                if (st.wMonth > 12)
                {
                    st.wYear += 1;
                    st.wMonth = 1;
                }

                mapAllTimes[min(LocalTimeToTime1970(&st), LocalTimeToTime1970(&stNow))] = false;
            }

            // 计算时间对
            bool bLastIsOn = false;
            DWORD dwLastTime = 0;

            for (map<DWORD, bool>::const_iterator it = mapAllTimes.begin(); it != mapAllTimes.end(); ++it)
            {
                if (!it->second && bLastIsOn)
                {
                    mapResult[dwLastTime] = it->first;
                }

                bLastIsOn = it->second;
                dwLastTime = it->first;
            }
        }

        SaveTimePairs(nYear, nMonth, mapResult);
        return mapResult;
    }

private:
    vector<DWORD> m_vectorOnTimes;
    vector<DWORD> m_vectorOffTimes;
};

class CPcActiveStatDialog
{
#define PCACTIVESTAT_OBJECT_PROP_NAME TEXT("__PcActiveStatObject")

public:
    CPcActiveStatDialog(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hParent)
        : m_hInstance(hInstance)
        , m_lpTemplate(lpTemplate)
        , m_hwndDlg(NULL)
        , m_hParent(hParent)
    {
    }

    INT_PTR DoModel()
    {
        return DialogBoxParam(m_hInstance, m_lpTemplate, m_hParent, PcActiveStatDialogProc, (LPARAM)this);
    }

private:
    static INT_PTR CALLBACK PcActiveStatDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_INITDIALOG)
        {
            SetProp(hwndDlg, PCACTIVESTAT_OBJECT_PROP_NAME, (HANDLE)lParam);
        }

        CPcActiveStatDialog *pPcActiveStatDialog = (CPcActiveStatDialog *)GetProp(hwndDlg, PCACTIVESTAT_OBJECT_PROP_NAME);

        if (pPcActiveStatDialog != NULL)
        {
            return pPcActiveStatDialog->PcActiveStatDialogPrivateProc(hwndDlg, uMsg, wParam, lParam);
        }

        switch (uMsg)
        {
        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            break;

        default:
            break;
        }

        return FALSE;
    }

    string FormatSystemTimeStruct(LPSYSTEMTIME lpSt)
    {
        return fmtA(
            "%04u-%02u-%02u %02u:%02u:%02u",
            lpSt->wYear,
            lpSt->wMonth,
            lpSt->wDay,
            lpSt->wHour,
            lpSt->wMinute,
            lpSt->wSecond);
    }

    void TestGetTimePairs()
    {
        int nYear = GetDlgItemInt(m_hwndDlg, IDC_YEAR, NULL, FALSE);
        int nMonth = (int)SendDlgItemMessage(m_hwndDlg, IDC_MONTH, CB_GETCURSEL, 0, 0) + 1;

        map<DWORD, DWORD> mapTimePairs = m_atr.GetYearMonthTimePairs(nYear, nMonth);
        stringstream ss;
        SYSTEMTIME st;
        DWORD dwTotalTime = 0;

        for (map<DWORD, DWORD>::const_iterator it = mapTimePairs.begin(); it != mapTimePairs.end(); ++it)
        {
            DWORD dwTimeSpan = it->second - it->first;
            DWORD dwHour = dwTimeSpan / 3600;
            DWORD dwMinute = dwTimeSpan / 60 % 60;

            dwTotalTime += dwTimeSpan;
            ss  <<FormatSystemTimeStruct(Time1970ToLocalTime(it->first, &st))<<"----"
                <<FormatSystemTimeStruct(Time1970ToLocalTime(it->second, &st))<<"----"
                <<dwHour<<":"<<dwMinute
                <<endl;
        }

        DWORD dwHour = dwTotalTime / 3600;
        DWORD dwMinute = dwTotalTime / 60 % 60;

        ss<<"Total: "<<dwHour<<":"<<dwMinute<<endl;

        MessageBoxA(m_hwndDlg, ss.str().c_str(), "pairs", MB_ICONINFORMATION);
    }

    void Show()
    {
        int nYear = GetDlgItemInt(m_hwndDlg, IDC_YEAR, NULL, FALSE);
        int nMonth = (int)SendDlgItemMessage(m_hwndDlg, IDC_MONTH, CB_GETCURSEL, 0, 0) + 1;
        map<DWORD, DWORD> mapTimePairs = m_atr.GetYearMonthTimePairs(nYear, nMonth);
        SYSTEMTIME st;

        if (mapTimePairs.empty())
        {
            MessageBoxFormat(m_hwndDlg, NULL, MB_ICONERROR, TEXT("没有查询到%u年%u月的记录"), nYear, nMonth);
            return;
        }

        GetLocalTime(&st);
        wstring strFilePath = ExportExcel(nYear, nMonth, mapTimePairs, st.wYear != nYear || st.wMonth != nMonth);

        if (strFilePath.empty() || !PathFileExistsW(strFilePath.c_str()))
        {
            MessageBox(m_hwndDlg, TEXT("导出excel失败"), NULL, MB_ICONERROR);
            return;
        }

        if ((int)ShellExecuteW(m_hwndDlg, L"open", strFilePath.c_str(), NULL, NULL, SW_SHOW) > 32)
        {
        }
        else
        {
            MessageBoxFormat(m_hwndDlg, NULL, MB_ICONERROR, TEXT("启动excel查看导出文件失败，文件路径为：%ls"), strFilePath.c_str());
            return;
        }
    }

    INT_PTR CALLBACK PcActiveStatDialogPrivateProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_INITDIALOG:{
            HICON hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));

            SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

            m_hwndDlg = hwndDlg;

            for (int i = 0; i < 12; ++i)
            {
                SendDlgItemMessage(hwndDlg, IDC_MONTH, CB_ADDSTRING, 0, (LPARAM)fmt(TEXT("%u月"), i + 1).c_str());
            }

            SYSTEMTIME st;

            GetLocalTime(&st);
            SetDlgItemInt(hwndDlg, IDC_YEAR, st.wYear, FALSE);
            SendDlgItemMessage(hwndDlg, IDC_MONTH, CB_SETCURSEL, st.wMonth - 1, 0);

            SendDlgItemMessage(hwndDlg, IDC_YEAR, EM_LIMITTEXT, 4, 0);

            break;}

        case WM_CLOSE:
            RemoveProp(hwndDlg, PCACTIVESTAT_OBJECT_PROP_NAME);
            EndDialog(hwndDlg, 0);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_SHOW)
            {
                SetFocus(GetDlgItem(m_hwndDlg, IDC_MONTH));
                Show();
            }
            break;

        case WM_LBUTTONDBLCLK:
            TestGetTimePairs();
            SetFocus(GetDlgItem(m_hwndDlg, IDC_MONTH));
            break;

        default:
            break;
        }

        return FALSE;
    }

private:
    HWND m_hwndDlg;
    HWND m_hParent;
    HINSTANCE m_hInstance;
    LPCTSTR m_lpTemplate;

    CActiveTimeReader m_atr;
};

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
    WCHAR szDllPath[MAX_PATH] = L"";

    ExpandEnvironmentStringsW(
        L"%tmp%\\Shilyx Studio\\libxl\\"
#ifdef _WIN64
        L"x64"
#else
        L"Win32"
#endif
        L"\\libxl.dll",
        szDllPath,
        RTL_NUMBER_OF(szDllPath));
    ForceContainingDirectoriesW(szDllPath);

    if (!PathFileExistsW(szDllPath))
    {
        DWORD dwSize = 0;
        LPCVOID lpBuffer = NULL;
        HRSRC hRsrc = FindResource(hInstance, MAKEINTRESOURCE(IDR_DLL), TEXT("dll"));

        if (hRsrc != NULL)
        {
            HGLOBAL hGlobal = LoadResource(hInstance, hRsrc);

            dwSize = SizeofResource(hInstance, hRsrc);

            if (hGlobal != NULL && dwSize > 0)
            {
                lpBuffer = LockResource(hGlobal);
            }
        }

        if (dwSize > 0 && lpBuffer != NULL)
        {
            HANDLE hFile = CreateFileW(szDllPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if (hFile != INVALID_HANDLE_VALUE)
            {
                DWORD dwBytesWritten = 0;

                WriteFile(hFile, lpBuffer, dwSize, &dwBytesWritten, NULL);

                CloseHandle(hFile);
            }
        }
    }

    if (LoadLibraryW(szDllPath) == NULL)
    {
        MessageBox(NULL, TEXT("无法加载libxl.dll"), NULL, MB_ICONERROR);
        return 0;
    }

    CPcActiveStatDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL).DoModel();
    return 0;
}