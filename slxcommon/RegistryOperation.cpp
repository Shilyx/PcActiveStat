#include "RegistryOperation.h"
#include <Shlwapi.h>

std::string RegGetStringA(HKEY hRootKey, LPCSTR lpRegPath, LPCSTR lpRegValue)
{
    HKEY hKey = NULL;
    std::string strResult;

    if (ERROR_SUCCESS == RegOpenKeyExA(hRootKey, lpRegPath, 0, KEY_QUERY_VALUE, &hKey))
    {
        DWORD dwSize = 0;

        if (ERROR_SUCCESS == RegQueryValueExA(hKey, lpRegValue, NULL, NULL, NULL, &dwSize) && dwSize > 0)
        {
            unsigned char *lpBuffer = (unsigned char *)malloc(dwSize);

            if (lpBuffer != NULL)
            {
                if (ERROR_SUCCESS == RegQueryValueExA(hKey, lpRegValue, NULL, NULL, lpBuffer, &dwSize))
                {
                    strResult = std::string((char *)lpBuffer, dwSize / sizeof(char) - 1);
                }

                free(lpBuffer);
            }
        }

        RegCloseKey(hKey);
    }

    return strResult;
}

std::wstring RegGetStringW(HKEY hRootKey, LPCWSTR lpRegPath, LPCWSTR lpRegValue)
{
    HKEY hKey = NULL;
    std::wstring strResult;

    if (ERROR_SUCCESS == RegOpenKeyExW(hRootKey, lpRegPath, 0, KEY_QUERY_VALUE, &hKey))
    {
        DWORD dwSize = 0;

        if (ERROR_SUCCESS == RegQueryValueExW(hKey, lpRegValue, NULL, NULL, NULL, &dwSize) && dwSize > 0)
        {
            unsigned char *lpBuffer = (unsigned char *)malloc(dwSize);

            if (lpBuffer != NULL)
            {
                if (ERROR_SUCCESS == RegQueryValueExW(hKey, lpRegValue, NULL, NULL, lpBuffer, &dwSize))
                {
                    strResult = std::wstring((wchar_t *)lpBuffer, dwSize / sizeof(wchar_t) - 1);
                }

                free(lpBuffer);
            }
        }

        RegCloseKey(hKey);
    }

    return strResult;
}

bool RegSetStringA(HKEY hRootKey, LPCSTR lpRegPath, LPCSTR lpRegValue, const std::string &strData)
{
    HKEY hKey = NULL;
    bool bSucceed = FALSE;

    if (ERROR_SUCCESS == RegCreateKeyExA(hRootKey, lpRegPath, 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL))
    {
        DWORD dwSize = 0;

        bSucceed = ERROR_SUCCESS == RegSetValueExA(hKey, lpRegValue, 0, REG_SZ, (const BYTE *)strData.c_str(), ((DWORD)strData.size() + 1) * sizeof(char));
        RegCloseKey(hKey);
    }

    return bSucceed;
}

bool RegSetStringW(HKEY hRootKey, LPCWSTR lpRegPath, LPCWSTR lpRegValue, const std::wstring &strData)
{
    HKEY hKey = NULL;
    bool bSucceed = FALSE;

    if (ERROR_SUCCESS == RegCreateKeyExW(hRootKey, lpRegPath, 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL))
    {
        DWORD dwSize = 0;

        bSucceed = ERROR_SUCCESS == RegSetValueExW(hKey, lpRegValue, 0, REG_SZ, (const BYTE *)strData.c_str(), ((DWORD)strData.size() + 1) * sizeof(wchar_t));
        RegCloseKey(hKey);
    }

    return bSucceed;
}