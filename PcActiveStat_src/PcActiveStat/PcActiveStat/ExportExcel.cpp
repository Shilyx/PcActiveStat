#include <slxcommon.h>
#include "ExportExcel.h"
#include <Shlwapi.h>
#include <libxl.h>
#include <string>
#include <vector>
#include <ShlObj.h>
#include <sstream>

#pragma comment(lib, "libxl.lib")

using namespace std;
using namespace libxl;

#define ABS(x) ((int)(x) < 0 ? (-(int)(x)) : (int)(x))

static wstring GetTimeStringFromSeconds(DWORD dwTimeSeconds)
{
    DWORD dwDays = dwTimeSeconds / (24 * 60 * 60);
    DWORD dwHours = dwTimeSeconds / (60 * 60) % 24;
    DWORD dwMinutes = dwTimeSeconds / 60 % 60;
    DWORD dwSeconds = dwTimeSeconds % 60;
    wstringstream ss;

    if (dwDays > 0)
    {
        ss<<dwDays<<L"��";
    }

    if (dwDays > 0 || dwHours > 0)
    {
        ss<<dwHours<<L"Сʱ";
    }

    if (dwDays > 0 || dwHours > 0 || dwMinutes > 0)
    {
        ss<<dwMinutes<<L"����";
    }

    ss<<dwSeconds<<L"��";

    return ss.str();
}

static BOOL ExportToExcelImage(int nYear, int nMonth, int nDayCount, const std::map<DWORD, DWORD> &mapData, BookW *pBook)
{
    WCHAR *lpWeekDayNames[] = {L"��", L"һ", L"��", L"��", L"��", L"��", L"��"};
    SYSTEMTIME stFirstDay = {0};

    stFirstDay.wYear = nYear;
    stFirstDay.wMonth = nMonth;
    stFirstDay.wDay = 1;

    Time1970ToLocalTime(LocalTimeToTime1970(&stFirstDay), &stFirstDay);

    int nBaseRow = 1;
    int nBaseCol = 0;
    int nColCount = nDayCount;

    SheetW *pSheet = pBook->addSheet(fmtW(L"%04u��%02u��ʾ��ͼ", nYear, nMonth).c_str());

    if (pSheet == NULL)
    {
        pBook->release();
        return FALSE;
    }

    pSheet->setDisplayGridlines(false);

    FormatW *pHeaderFormat = pBook->addFormat();
    FontW* pHeaderFont = pBook->addFont();
    pHeaderFormat->setAlignH(ALIGNH_CENTER);
    pHeaderFormat->setBorder(BORDERSTYLE_THIN);
    pHeaderFormat->setFillPattern(FILLPATTERN_SOLID);
    pHeaderFormat->setPatternForegroundColor(COLOR_YELLOW);
    pHeaderFont->setBold(true);
    pHeaderFont->setSize(10);
    pHeaderFont->setName(TEXT("����"));
    pHeaderFormat->setFont(pHeaderFont);
    pHeaderFormat->setWrap(true);
    pHeaderFormat->setAlignV(ALIGNV_CENTER);

    FormatW *pDataFormat = pBook->addFormat();
    pDataFormat->setBorderRight(BORDERSTYLE_DOTTED);
    pDataFormat->setFillPattern(FILLPATTERN_SOLID);
    pDataFormat->setPatternForegroundColor(COLOR_LIGHTBLUE);
    pDataFormat->setWrap(true);

    FormatW *pBlankFormat = pBook->addFormat();
    pBlankFormat->setBorderRight(BORDERSTYLE_DOTTED);
    pBlankFormat->setFillPattern(FILLPATTERN_SOLID);
    pBlankFormat->setPatternForegroundColor(COLOR_IVORY_CF);
    pBlankFormat->setWrap(true);

    FormatW *pBlankFormatOdd = pBook->addFormat();
    pBlankFormatOdd->setBorderRight(BORDERSTYLE_DOTTED);
    pBlankFormatOdd->setFillPattern(FILLPATTERN_SOLID);
    pBlankFormatOdd->setPatternForegroundColor(COLOR_LIGHTTURQUOISE_CF);
    pBlankFormatOdd->setWrap(true);

    // ����������ϣ�
    pSheet->setCol(nBaseCol + 1, nBaseCol + nColCount, 2.5f);

    for (int i = 0; i < nColCount; ++i)
    {
        pSheet->writeNum(nBaseRow, nBaseCol + 1 + i, i + 1, pHeaderFormat);
    }

    pSheet->writeStr(nBaseRow, nBaseCol, fmtW(L"%u��", nMonth).c_str(), pHeaderFormat);

    for (int i = 0, nWeekDayIndex = stFirstDay.wDayOfWeek; i < nColCount; ++i, ++nWeekDayIndex)
    {
        pSheet->writeStr(nBaseRow + 1, nBaseCol + 1 + i, lpWeekDayNames[nWeekDayIndex % RTL_NUMBER_OF(lpWeekDayNames)], pHeaderFormat);
    }

    pSheet->writeStr(nBaseRow + 1, nBaseCol, L"��", pHeaderFormat);

    for (int i = 0; i < 15 * 24; ++i)
    {
        pSheet->setRow(i + nBaseRow + 2, 1.5f);
        pSheet->writeStr(i + nBaseRow + 2, nBaseCol, L"", pHeaderFormat);
    }

    for (int i = 0; i < 24; ++i)
    {
        pSheet->setMerge(i * 15 + nBaseRow + 2, (1 + i) * 15 + nBaseRow + 1, nBaseCol, nBaseCol);
    }

    for (int i = 0; i < 24; ++i)
    {
        pSheet->writeStr(i * 15 + nBaseRow + 2, nBaseCol, fmt(TEXT("%u:00"), i).c_str(), pHeaderFormat);
    }

    // ��䱳������
    for (int nCol = nBaseCol + 1; nCol < nBaseCol + 1 + nColCount; ++nCol)
    {
        for (int nRow = nBaseRow + 2, nTimeUnit = 0; nRow < nBaseRow + 2 + 15 * 24; ++nRow, ++nTimeUnit)
        {
            if (nTimeUnit / 15 % 2 == 0)
            {
                pSheet->writeStr(nRow, nCol, L"", pBlankFormat);
            }
            else
            {
                pSheet->writeStr(nRow, nCol, L"", pBlankFormatOdd);
            }
        }
    }

    // �������
    if (!mapData.empty())
    {
        map<DWORD, DWORD>::const_iterator it = mapData.begin();

        for (int nCol = nBaseCol + 1, nDay = 1; nCol < nBaseCol + 1 + nColCount && it != mapData.end(); ++nCol, ++nDay)
        {
            DWORD dwTimeInThisDay = 0;
            SYSTEMTIME st = {0};

            st.wYear = nYear;
            st.wMonth = nMonth;
            st.wDay = nDay;

            dwTimeInThisDay = LocalTimeToTime1970(&st);

            for (int nRow = nBaseRow + 2, nTimeUnit = 0; nRow < nBaseRow + 2 + 15 * 24; ++nRow, ++nTimeUnit, dwTimeInThisDay += 4 * 60)
            {
                while (it != mapData.end())
                {
                    if (it->second < dwTimeInThisDay)
                    {
                        ++it;
                    }
                    else
                    {
                        if (it->first <= dwTimeInThisDay)
                        {
                            pSheet->writeStr(nRow, nCol, L"", pDataFormat);
                        }

                        break;
                    }
                }

                if (it == mapData.end())
                {
                    break;
                }
            }
        }
    }

    // ��������£�
    for (int i = 0, nWeekDayIndex = stFirstDay.wDayOfWeek; i < nColCount; ++i, ++nWeekDayIndex)
    {
        pSheet->writeStr(nBaseRow + 15 * 24 + 2, nBaseCol + 1 + i, lpWeekDayNames[nWeekDayIndex % RTL_NUMBER_OF(lpWeekDayNames)], pHeaderFormat);
    }

    pSheet->writeStr(nBaseRow + 15 * 24 + 2, nBaseCol, L"��", pHeaderFormat);

    // ���ᵥԪ��
    pSheet->split(nBaseRow + 2, nBaseCol + 1);

    return TRUE;
}

static BOOL ExportToExcelTable(int nYear, int nMonth, int nDayCount, const std::map<DWORD, DWORD> &mapData, BookW *pBook)
{
    int nBaseRow = 1;
    int nBaseCol = 0;
    int nColCount = 4;

    SheetW *pSheet = pBook->addSheet(fmtW(L"%04u��%02u��ͳ�Ʊ�", nYear, nMonth).c_str());

    if (pSheet == NULL)
    {
        pBook->release();
        return FALSE;
    }

    FormatW *pHeaderFormat = pBook->addFormat();
    FontW* pHeaderFont = pBook->addFont();
    pHeaderFormat->setAlignH(ALIGNH_LEFT);
    pHeaderFormat->setBorder(BORDERSTYLE_MEDIUM);
    pHeaderFormat->setFillPattern(FILLPATTERN_SOLID);
    pHeaderFormat->setPatternForegroundColor(COLOR_YELLOW);
    pHeaderFont->setColor(COLOR_RED);
    pHeaderFont->setBold(true);
    pHeaderFont->setSize(10);
    pHeaderFont->setName(TEXT("����"));
    pHeaderFormat->setFont(pHeaderFont);
    pHeaderFormat->setWrap(true);
    pHeaderFormat->setAlignV(ALIGNV_CENTER);

    FormatW *pDefaultFormat = pBook->addFormat();
    FontW* pDefaultFont = pBook->addFont();
    pDefaultFormat->setAlignH(ALIGNH_LEFT);
    pDefaultFormat->setBorder(BORDERSTYLE_THIN);
    pDefaultFont->setColor(COLOR_BLUE);
    pDefaultFont->setBold(true);
    pDefaultFont->setSize(10);
    pDefaultFont->setName(TEXT("����"));
    pDefaultFormat->setFont(pDefaultFont);
    pDefaultFormat->setWrap(true);
    pDefaultFormat->setAlignV(ALIGNV_CENTER);

    pSheet->setCol(nBaseCol, nBaseCol + nColCount - 1, 26.0f);

    // ÿ�����Ϣ
    pSheet->writeStr(nBaseRow, nBaseCol + 0, L"����", pHeaderFormat);
    pSheet->writeStr(nBaseRow, nBaseCol + 1, L"����ʱ��", pHeaderFormat);
    pSheet->writeStr(nBaseRow, nBaseCol + 2, L"�ػ�ʱ��", pHeaderFormat);
    pSheet->writeStr(nBaseRow, nBaseCol + 3, L"����ʱ��", pHeaderFormat);

    int nRowIndex = nBaseRow + 1;
    SYSTEMTIME stBegin;
    SYSTEMTIME stEnd;
    SYSTEMTIME stNow;
    DWORD dwNow = 0;

    GetLocalTime(&stNow);
    dwNow = LocalTimeToTime1970(&stNow);

    for (map<DWORD, DWORD>::const_iterator it = mapData.begin(); it != mapData.end(); ++it, ++nRowIndex)
    {
        DWORD dwTimeRange = it->second - it->first;

        Time1970ToLocalTime(it->first, &stBegin);
        Time1970ToLocalTime(it->second, &stEnd);

        wstring strDate = fmtW(L"%04u-%02u-%02u", stBegin.wYear, stBegin.wMonth, stBegin.wDay);
        wstring strBeginTime = fmtW(L"%02u:%02u:%02u", stBegin.wHour, stBegin.wMinute, stBegin.wSecond);
        wstring strEndTime = fmtW(L"%02u:%02u:%02u", stEnd.wHour, stEnd.wMinute, stEnd.wSecond);

        if (stBegin.wDay != stEnd.wDay)
        {
            strEndTime += fmtW(L"(%04u-%02u-%02u)", stEnd.wYear, stEnd.wMonth, stEnd.wDay);
        }

        if (ABS(it->second - dwNow) < 30)
        {
            strEndTime += L"(NOW)";
        }

        pSheet->writeStr(nRowIndex, nBaseCol, strDate.c_str(), pDefaultFormat);
        pSheet->writeStr(nRowIndex, nBaseCol + 1, strBeginTime.c_str(), pDefaultFormat);
        pSheet->writeStr(nRowIndex, nBaseCol + 2, strEndTime.c_str(), pDefaultFormat);
        pSheet->writeStr(nRowIndex, nBaseCol + 3, GetTimeStringFromSeconds(dwTimeRange).c_str(), pDefaultFormat);
    }

    // �ϲ����ڵ�Ԫ��
    for (int i = nBaseRow + 1; i < nRowIndex; ++i)
    {
        wstring strText = pSheet->readStr(i, nBaseCol);
        int j = i + 1;

        for (; j < nRowIndex; ++j)
        {
            if (lstrcmpiW(strText.c_str(), pSheet->readStr(j, nBaseCol)) != 0)
            {
                break;
            }
        }

        if (j > i + 1)
        {
            pSheet->setMerge(i, j - 1, nBaseCol, nBaseCol);

            i = j - 1;
        }
    }

    // ��ȡͳ����Ϣ
    DWORD dwTotalTimeRange = 0;         // ��ʱ��
    DWORD dwTimeRangeAVG = -1;
    DWORD dwTotalOnTime = 0;            // �ܿ���ʱ��
    DWORD dwOnTimeAVG = -1;
    DWORD dwTotalOffTime = 0;           // �ܹػ�ʱ��
    DWORD dwOffTimeAVG = -1;
    map<int, DWORD> mapDayOnTime;       // ÿ������翪��ʱ��
    DWORD dwDayOnTimeAVG = -1;
    map<int, DWORD> mapDayOffTime;      // ÿ�������ػ�ʱ��
    DWORD dwDayOffTimeAVG = -1;
    set<int> setActiveDays;             // ������һʱ�̴��ڿ���״̬������
    DWORD dwDayTimeRangeAVG = -1;
    DWORD dwDayBeginTimes[32];          // ÿ��Ŀ�ʼʱ�䣬base 1
    DWORD dwDayEndTimes[32];            // ÿ��Ľ���ʱ�䣬base 1
    SYSTEMTIME stTempInMonth = {0};

    stTempInMonth.wYear = nYear;
    stTempInMonth.wMonth = nMonth;

    for (int i = 1; i <= nDayCount; ++i)
    {
        stTempInMonth.wDay = i;
        dwDayBeginTimes[i] = LocalTimeToTime1970(&stTempInMonth);
        dwDayEndTimes[i] = dwDayBeginTimes[i] + 24 * 60 * 60;
    }

    for (map<DWORD, DWORD>::const_iterator it = mapData.begin(); it != mapData.end(); ++it)
    {
        DWORD dwOnTime = it->first;
        DWORD dwOffTime = it->second;
        SYSTEMTIME stOnTime;
        SYSTEMTIME stOffTime;

        Time1970ToLocalTime(dwOnTime, &stOnTime);
        Time1970ToLocalTime(dwOffTime, &stOffTime);

        dwTotalTimeRange += (dwOffTime - dwOnTime);
        dwTotalOnTime += dwOnTime - dwDayBeginTimes[stOnTime.wDay];
        dwTotalOffTime += dwOffTime - dwDayBeginTimes[stOffTime.wDay];

        if (mapDayOnTime.count(stOnTime.wDay) == 0)
        {
            mapDayOnTime[stOnTime.wDay] = dwOnTime - dwDayBeginTimes[stOnTime.wDay];
        }

        mapDayOffTime[stOffTime.wDay] = dwOffTime - dwDayBeginTimes[stOffTime.wDay];

        for (int i = stOnTime.wDay; i <= stOffTime.wDay; ++i)
        {
            setActiveDays.insert(i);
        }
    }

    if (!mapData.empty())
    {
        dwTimeRangeAVG = dwTotalTimeRange / (int)mapData.size();
        dwOnTimeAVG = dwTotalOnTime / (int)mapData.size();
        dwOffTimeAVG = dwTotalOffTime / (int)mapData.size();
    }

    if (!setActiveDays.empty())
    {
        dwDayTimeRangeAVG = dwTotalTimeRange / (int)setActiveDays.size();
    }

    if (!mapDayOnTime.empty())
    {
        dwDayOnTimeAVG = 0;

        for (map<int, DWORD>::iterator it = mapDayOnTime.begin(); it != mapDayOnTime.end(); ++it)
        {
            dwDayOnTimeAVG += it->second;
        }

        dwDayOnTimeAVG /= (int)mapDayOnTime.size();
    }

    if (!mapDayOffTime.empty())
    {
        dwDayOffTimeAVG = 0;

        for (map<int, DWORD>::iterator it = mapDayOffTime.begin(); it != mapDayOffTime.end(); ++it)
        {
            dwDayOffTimeAVG += it->second;
        }

        dwDayOffTimeAVG /= (int)mapDayOffTime.size();
    }

    // չʾ
    vector<pair<wstring, wstring> > vectorStatInfos;

    vectorStatInfos.push_back(make_pair(L"��ʱ��", GetTimeStringFromSeconds(dwTotalTimeRange)));
    vectorStatInfos.push_back(make_pair(L"������", fmtW(L"%lu", (int)setActiveDays.size())));
    vectorStatInfos.push_back(make_pair(L"ƽ��ÿ�����翪��ʱ��", dwDayOnTimeAVG == -1 ? L"N/A" : fmtW(L"%02u:%02u:%02u", dwDayOnTimeAVG / 3600, dwDayOnTimeAVG / 60 % 60, dwDayOnTimeAVG % 60)));
    vectorStatInfos.push_back(make_pair(L"ƽ��ÿ������ػ�ʱ��", dwDayOffTimeAVG == -1 ? L"N/A" : fmtW(L"%02u:%02u:%02u", dwDayOffTimeAVG / 3600, dwDayOffTimeAVG / 60 % 60, dwDayOffTimeAVG % 60)));
    vectorStatInfos.push_back(make_pair(L"ƽ��ÿ������ʱ��", GetTimeStringFromSeconds(dwDayTimeRangeAVG)));
    vectorStatInfos.push_back(make_pair(L"�ܿ��ػ�����", fmtW(L"%lu", (int)mapData.size())));
    vectorStatInfos.push_back(make_pair(L"ƽ��ÿ�ο��ػ�����ʱ��", mapData.empty() ? L"N/A" : fmtW(L"%02u:%02u:%02u", dwOnTimeAVG / 3600, dwOnTimeAVG / 60 % 60, dwOnTimeAVG % 60)));
    vectorStatInfos.push_back(make_pair(L"ƽ��ÿ�ο��ػ��ػ�ʱ��", mapData.empty() ? L"N/A" : fmtW(L"%02u:%02u:%02u", dwOffTimeAVG / 3600, dwOffTimeAVG / 60 % 60, dwOffTimeAVG % 60)));
    vectorStatInfos.push_back(make_pair(L"ƽ��ÿ�ο��ػ�����ʱ��", mapData.empty() ? L"N/A" : GetTimeStringFromSeconds(dwTimeRangeAVG)));

    pSheet->setCol(nBaseCol + 5, nBaseCol + 6, 26.0f);

    for (vector<pair<wstring, wstring> >::size_type i = 0; i < vectorStatInfos.size(); ++i)
    {
        pSheet->writeStr(nBaseRow + (int)i + 1, nBaseCol + 5, vectorStatInfos.at(i).first.c_str(), pHeaderFormat);
        pSheet->writeStr(nBaseRow + (int)i + 1, nBaseCol + 6, vectorStatInfos.at(i).second.c_str(), pDefaultFormat);
    }

    return TRUE;
}

static BOOL ExportToExcelFile(int nYear, int nMonth, int nDayCount, const std::map<DWORD, DWORD> &mapData, const wstring &srtFilePath)
{
    BOOL bSucceed = FALSE;
    BookW *pBook = xlCreateXMLBook();

    if (pBook == NULL)
    {
        return bSucceed;
    }

    wstring strName = RegGetStringW(HKEY_CURRENT_USER, L"Software\\Shilyx Studio\\PcActiveStat\\libxl", L"name");
    wstring strKey = RegGetStringW(HKEY_CURRENT_USER, L"Software\\Shilyx Studio\\PcActiveStat\\libxl", L"key");

    RegSetStringW(HKEY_CURRENT_USER, L"Software\\Shilyx Studio\\PcActiveStat\\libxl", L"name", strName);
    RegSetStringW(HKEY_CURRENT_USER, L"Software\\Shilyx Studio\\PcActiveStat\\libxl", L"key", strKey);

    pBook->setKey(strName.c_str(), strKey.c_str());

    if (ExportToExcelTable(nYear, nMonth, nDayCount, mapData, pBook) &&
        ExportToExcelImage(nYear, nMonth, nDayCount, mapData, pBook))
    {
        bSucceed = !!pBook->save(srtFilePath.c_str());
    }

    pBook->release();

    return bSucceed;
}

std::wstring ExportExcel(int nYear, int nMonth, const std::map<DWORD, DWORD> &mapData, bool bEnabledCache)
{
    if (nMonth < 1 || nMonth > 12)
    {
        return L"";
    }

    // ��������
    static int s_nMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int nDayCount = s_nMonthDays[nMonth - 1];

    if (nMonth == 2)
    {
        if ((nYear % 4 == 0 && nYear % 100 != 0) || nYear % 400 == 0)
        {
            nDayCount = 29;
        }
    }

    // ƴ���ļ�·��
    WCHAR szFilePath[MAX_PATH];

    GetModuleFileNameW(GetModuleHandle(NULL), szFilePath, RTL_NUMBER_OF(szFilePath));
    PathAppendW(szFilePath, L"\\..\\PcActiveStatFiles\\Cache");
    SHCreateDirectory(NULL, szFilePath);
    PathAppendW(szFilePath, fmtW(L"\\stat_%04u_%02u.xlsx", nYear, nMonth).c_str());

    if (!ExportToExcelFile(nYear, nMonth, nDayCount, mapData, szFilePath))
    {
        DeleteFileW(szFilePath);
        return L"";
    }

    return szFilePath;
}
