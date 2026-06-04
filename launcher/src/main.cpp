#include <filesystem>
#include <iostream>
#include <string>

#include <windows.h>

#pragma comment(lib, "user32.lib")

namespace GTASA::SDK::Launcher {

    constexpr const char* kDllName = "GTASA_SDK.dll";

    std::filesystem::path getCurrentDirectoryPath()
    {
        char currentDir[MAX_PATH] = {0};
        GetCurrentDirectoryA(MAX_PATH, currentDir);
        return std::filesystem::path(currentDir);
    }

    std::string openBrowser()
    {
        OPENFILENAMEA ofn;
        char szFile[260] = {0};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);

        ofn.lpstrFilter = "Executable Files\0*.exe\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn) == TRUE)
            return std::string(szFile);

        return std::string();
    }

    bool launchSuspendedProcess(const std::string& exePath, const std::string& gameDir,
        PROCESS_INFORMATION& processInfo)
    {
        STARTUPINFOA startupInfo = {sizeof(startupInfo)};
        return CreateProcessA(exePath.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL,
            gameDir.c_str(), &startupInfo, &processInfo) == TRUE;
    }

    bool injectDll(HANDLE processHandle, const std::string& dllPath)
    {
        LPVOID allocatedMem = VirtualAllocEx(processHandle, nullptr, dllPath.length() + 1,
            MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!allocatedMem)
            return false;

        SIZE_T bytesWritten = 0;
        if (!WriteProcessMemory(processHandle, allocatedMem, dllPath.c_str(), dllPath.length() + 1,
                                    &bytesWritten) || bytesWritten != dllPath.length() + 1)
        {
            VirtualFreeEx(processHandle, allocatedMem, 0, MEM_RELEASE);
            return false;
        }

        HANDLE remoteThread = CreateRemoteThread(processHandle, nullptr, 0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA), allocatedMem, 0, nullptr);
        if (!remoteThread)
        {
            VirtualFreeEx(processHandle, allocatedMem, 0, MEM_RELEASE);
            return false;
        }

        WaitForSingleObject(remoteThread, INFINITE);
        CloseHandle(remoteThread);
        VirtualFreeEx(processHandle, allocatedMem, 0, MEM_RELEASE);
        return true;
    }
}

using namespace GTASA::SDK::Launcher;

int main()
{
    auto currentDir = getCurrentDirectoryPath();
    std::string dllPath = (currentDir / kDllName).string();

    std::cout << "[*] Welcome to the Custom Launcher!\n";

    if (!std::filesystem::exists(dllPath))
    {
        std::cout << "[-] ERROR: DLL file not found at: " << dllPath << "\n";
        std::cout << "[*] Please make sure the DLL is in the correct directory.\n";
        system("pause");
        return 1;
    }

    std::cout << "[+] Excellent! DLL found successfully at: " << dllPath << "\n";

    std::cout << "[*] Please select the GTA San Andreas executable (gta_sa.exe) to launch.\n";
    std::string exePath = openBrowser();

    if (exePath.empty())
    {
        std::cout << "[-] Selection canceled.\n";
        return 1;
    }

    std::string gameDir = exePath.substr(0, exePath.find_last_of("\\/"));

    std::cout << "[*] Selected file path: " << exePath << std::endl;

    std::cout << "[*] Starting Custom Launcher (Suspended Mode)...\n";

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (!launchSuspendedProcess(exePath, gameDir, pi))
    {
        std::cout << "[-] Failed to start gta_sa.exe!\n";
        system("pause");
        return 1;
    }

    std::cout << "[*] Injecting GTASA_SDK.dll before game wakes up...\n";

    if (!injectDll(pi.hProcess, dllPath))
    {
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        system("pause");
        return 1;
    }

    std::cout << "[+] DLL Injected.\n";


    std::cout << "[*] Waking up the game...\n";
    ResumeThread(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
