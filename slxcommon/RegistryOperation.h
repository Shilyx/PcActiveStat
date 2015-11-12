#ifndef _REGISTRYOPERATION_H
#define _REGISTRYOPERATION_H

#include <string>
#include <Windows.h>

#ifdef _UNICODE
#define RegGetString RegGetStringW
#else
#define RegGetString RegGetStringA
#endif

std::string RegGetStringA(HKEY hRootKey, LPCSTR lpRegPath, LPCSTR lpRegValue);
std::wstring RegGetStringW(HKEY hRootKey, LPCWSTR lpRegPath, LPCWSTR lpRegValue);

bool RegSetStringA(HKEY hRootKey, LPCSTR lpRegPath, LPCSTR lpRegValue, const std::string &strData);
bool RegSetStringW(HKEY hRootKey, LPCWSTR lpRegPath, LPCWSTR lpRegValue, const std::wstring &strData);

#endif /* _REGISTRYOPERATION_H */
