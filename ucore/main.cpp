#include "pch.h"
#include "Windows.h"
#include "Universe.h"


int main(int argc, char *argv[])
{

    wchar_t fullPath[MAX_PATH];
    wchar_t drive[MAX_PATH];
    wchar_t dir[MAX_PATH];

    char mystr[] = "Hello World";
    


    DWORD r = GetModuleFileNameW(NULL, (wchar_t*)fullPath, MAX_PATH);
    printf("GetModuleFileNameW returned %d\n", r);
    wprintf(L"EXE full path is %ls\n", fullPath);

    printf("%c\n", fullPath[0]);
    printf("%c\n", fullPath[1]);
    printf("%c\n", fullPath[2]);
    printf("%c\n", fullPath[3]);
    printf("%c\n", fullPath[4]);

    printf("mystr = %s\n", mystr);

    _wsplitpath_s(
        (const wchar_t*)fullPath,
        (wchar_t*)drive, MAX_PATH,
        (wchar_t*)dir, MAX_PATH,
        NULL, 0,
        NULL, 0);

    wprintf(L"Directory of EXE is %ls\n", dir);

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
        Sleep(10000);
    }
    return retval;
}