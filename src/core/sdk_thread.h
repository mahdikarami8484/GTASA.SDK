#pragma once

#include <Windows.h>

#include "core/sdk_runtime.h"

DWORD WINAPI SDKThread(LPVOID)
{
    GTASA::SDK::SDKRuntime::instance().init();
    return 0;
}