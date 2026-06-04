#include <windows.h>

#include <commdlg.h>

#include <filesystem>
#include <iostream>
#include <string>

namespace GTASA::SDK::Launcher
{

    constexpr const wchar_t* kDllName = L"GTASA_SDK.dll";

    std::filesystem::path getCurrentDirectoryPath()
    {
        std::wstring exePath(32767, L'\0');
        const DWORD len = GetModuleFileNameW(nullptr, exePath.data(), exePath.size());
        if (len > 0 && len < exePath.size())
        {
            exePath.resize(len);
            return std::filesystem::path(exePath).parent_path();
        }

        return std::filesystem::current_path();
    }

    std::wstring openBrowser()
    {
        OPENFILENAMEW ofn;
        wchar_t szFile[260] = {0};

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = static_cast<DWORD>(_countof(szFile));

        ofn.lpstrFilter = L"Executable Files\0*.exe\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameW(&ofn) == TRUE) return std::wstring(szFile);

        return std::wstring();
    }

    bool launchSuspendedProcess(const std::wstring& exePath, const std::wstring& gameDir,
                                PROCESS_INFORMATION& processInfo)
    {
        STARTUPINFOW startupInfo = {sizeof(startupInfo)};
        return CreateProcessW(exePath.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL,
                              gameDir.c_str(), &startupInfo, &processInfo) == TRUE;
    }

    bool injectDll(HANDLE processHandle, const std::wstring& dllPath)
    {
        HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
        if (!hKernel32) return false;

        LPTHREAD_START_ROUTINE pLoadLibraryW =
            reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hKernel32, "LoadLibraryW"));
        if (!pLoadLibraryW) return false;

        const SIZE_T dllPathBytes = (dllPath.length() + 1) * sizeof(wchar_t);
        LPVOID allocatedMem = VirtualAllocEx(processHandle, nullptr, dllPathBytes,
                                             MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!allocatedMem) return false;

        SIZE_T bytesWritten = 0;
        if (!WriteProcessMemory(processHandle, allocatedMem, dllPath.c_str(), dllPathBytes,
                                &bytesWritten) ||
            bytesWritten != dllPathBytes)
        {
            VirtualFreeEx(processHandle, allocatedMem, 0, MEM_RELEASE);
            return false;
        }

        HANDLE remoteThread =
            CreateRemoteThread(processHandle, nullptr, 0, pLoadLibraryW, allocatedMem, 0, nullptr);
        if (!remoteThread)
        {
            VirtualFreeEx(processHandle, allocatedMem, 0, MEM_RELEASE);
            return false;
        }

        const DWORD waitResult = WaitForSingleObject(remoteThread, 15000); // Wait up to 15 seconds
        DWORD remoteExitCode = 0;
        const bool gotExitCode = (waitResult == WAIT_OBJECT_0) &&
                                 (GetExitCodeThread(remoteThread, &remoteExitCode) == TRUE);
        CloseHandle(remoteThread);
        VirtualFreeEx(processHandle, allocatedMem, 0, MEM_RELEASE);
        return gotExitCode && (remoteExitCode != 0);
    }
} // namespace GTASA::SDK::Launcher

using namespace GTASA::SDK::Launcher;

int main()
{
    auto currentDir = getCurrentDirectoryPath();
    std::wstring dllPath = (currentDir / kDllName).wstring();

    std::cout << "[*] Welcome to the Custom Launcher!\n";

    if (!std::filesystem::exists(std::filesystem::path(dllPath)))
    {
        std::wcout << L"[-] ERROR: DLL file not found at: " << dllPath << L"\n";
        std::cout << "[*] Please make sure the DLL is in the correct directory.\n";
        std::cout << "[*] Press Enter to exit...\n";
        std::cin.get();
        return 1;
    }

    std::wcout << L"[+] Excellent! DLL found successfully at: " << dllPath << L"\n";

    std::cout << "[*] Please select the GTA San Andreas executable (gta_sa.exe) to launch.\n";
    std::wstring exePath = openBrowser();

    if (exePath.empty())
    {
        std::cout << "[-] Selection canceled.\n";
        std::cout << "[*] Press Enter to exit...\n";
        std::cin.get();
        return 1;
    }

    std::wstring gameDir = std::filesystem::path(exePath).parent_path().wstring();

    std::wcout << L"[*] Selected file path: " << exePath << std::endl;

    std::cout << "[*] Starting Custom Launcher (Suspended Mode)...\n";

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (!launchSuspendedProcess(exePath, gameDir, pi))
    {
        std::wcout << L"[-] Failed to start process: " << exePath << L" (GetLastError="
                   << GetLastError() << L")\n";
        std::cout << "[*] Press Enter to exit...\n";
        std::cin.get();
        return 1;
    }

    std::cout << "[*] Injecting GTASA_SDK.dll before game wakes up...\n";

    if (!injectDll(pi.hProcess, dllPath))
    {
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        std::cout << "[*] Press Enter to exit...\n";
        std::cin.get();
        return 1;
    }

    std::cout << "[+] DLL Injected.\n";

    std::cout << "[*] Waking up the game...\n";
    ResumeThread(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
