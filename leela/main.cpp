#include "Windows.h"
#include "ShlObj.h"
#include "Universe.h"
#include <spdlog/spdlog.h>
#include "spdlog/sinks/rotating_file_sink.h"

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




Universe * g_universe = new Universe();  // global instance accessible to all objects
Space* g_space = &g_universe->space;

int main(int argc, char *argv[])
//int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{

#ifdef _WIN32
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif




    //-------------------------------------------

    CHAR path[MAX_PATH];
    std::string appName = "Leela";

    // Get the path to the AppData\Local folder
    if (SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path) != S_OK) {
        std::cerr << "Failed to get AppData Local folder path" << std::endl;
    }

    std::string logFolderPath = std::string(path) + "\\" + appName + "\\" + "Logs";
    std::string logFilePath = logFolderPath + "\\" + "leela.log";

    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");


    // Create file and console sink
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFilePath, 5 * 1024 * 1024, 3);
    auto console_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(file_sink);
    sinks.push_back(console_sink);
    auto logger = std::make_shared<spdlog::logger>("mainlog", sinks.begin(), sinks.end());
    spdlog::set_default_logger(logger);
    spdlog::flush_every(std::chrono::seconds(5));

    //-------------------------------------------
    
    if (!changeDirToParentOfExecutable())
    {
        return 1;
    }

    auto retval = g_universe->run();

    if (retval != 0)
    {
        Sleep(1000);
    }

    spdlog::shutdown();

    return retval;
}