#include "pch.h"
#include "Windows.h"
#include "Universe.h"


int main(int argc, char *argv[])
{

    wchar_t fullPath[MAX_PATH];
    wchar_t drive[MAX_PATH];
    wchar_t dir[MAX_PATH];

    DWORD r = GetModuleFileNameW(NULL, (wchar_t*)fullPath, MAX_PATH);
    printf("GetModuleFileNameW returned %d\n", r);
    wprintf(L"EXE full path is %ls\n", fullPath);

    _wsplitpath_s(
        (const wchar_t*)fullPath,
        (wchar_t*)drive, MAX_PATH,
        (wchar_t*)dir, MAX_PATH,
        NULL, 0,
        NULL, 0);

    _wmakepath_s((wchar_t*)fullPath, MAX_PATH, (wchar_t*)drive, (wchar_t*)dir, NULL, NULL);
    wprintf(L"Setting current directory to %ls\n", fullPath);


    if (!SetCurrentDirectory((LPCTSTR)fullPath))
    {
        printf("ERROR: Couldn't change current working directory");
        return 1;
    }

    Universe app;
    auto retval = app.run();

    if (retval != 0)
    {
        Sleep(1000);
    }
    return retval;
}