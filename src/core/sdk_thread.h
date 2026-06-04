#pragma once

#include "core/sdk_runtime.h"

#include <Windows.h>

DWORD WINAPI SDKThread(LPVOID)
{
    GTASA::SDK::SDKRuntime::instance().init();
    return 0;
}