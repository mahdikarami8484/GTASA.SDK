#include "Windows.h"
#include "core/sdk_thread.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{

    using namespace GTASA::SDK;

    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        /* Create SDK Thread */
        CreateThread(NULL, 0, SDKThread, NULL, 0, NULL);

        break;

    case DLL_PROCESS_DETACH:

        /* Shutdown sdk */
        SDKRuntime::instance().shutdown();

        break;
    default:
        break;
    }

    return true;
}