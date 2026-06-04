#pragma once
#include <windows.h> // برای LPCSTR

namespace GTASA
{
    namespace SDK
    {
        namespace Internal
        {
            namespace Functions
            {
                namespace CHUD
                {
                    using sendHelpMessage_t = void(__cdecl*)(const char*, bool, bool, bool);
                    inline sendHelpMessage_t sendHelpMessage = nullptr;
                } // namespace CHUD

                namespace System
                {
                    // --- ProcessFrame ---
                    using tProcessFrame = int(__cdecl*)(int command, int param);
                    inline tProcessFrame oProcessFrame = nullptr;

                    // --- strcpy ---
                    using t_strcpy = void(__cdecl*)(int destination, int source,
                                                    unsigned short param_3);
                    inline t_strcpy o_strcpy = nullptr;

                    // --- ScreenLoad ---
                    using tScreenLoad = void(__stdcall*)(void*, LPCSTR);
                    inline tScreenLoad oScreenLoad = nullptr;
                } // namespace System
            } // namespace Functions
        } // namespace Internal
    } // namespace SDK
} // namespace GTASA
