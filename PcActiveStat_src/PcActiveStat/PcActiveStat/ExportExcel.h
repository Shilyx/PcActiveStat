#ifndef _EXPORTEXCEL_H
#define _EXPORTEXCEL_H

#include <Windows.h>
#include <string>
#include <map>

std::wstring ExportExcel(int nYear, int nMonth, const std::map<DWORD, DWORD> &vectorData, bool bEnabledCache);

#endif /* _EXPORTEXCEL_H */
