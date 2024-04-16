#include "Windows.h"
#include "Universe.h"
#include <spdlog/spdlog.h>

/*
 * Change to the parent directory of the executable we are running from.
 */
bool changeDirToParentOfExecutable()
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
        return false;
    }
    else
    {
        return true;
    }
}

#undef main

Universe * g_universe = new Universe();  // global instance accessible to all objects
Space* g_space = &g_universe->space;

int main(int argc, char *argv[])
{
    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
    
    if (!changeDirToParentOfExecutable())
    {
        return 1;
    }

    auto retval = g_universe->run();

    if (retval != 0)
    {
        Sleep(1000);
    }
    return retval;
}