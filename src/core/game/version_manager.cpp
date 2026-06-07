#include <windows.h>

#include "version_manager.h"

#include "core/logging/logger.hpp"
#include "offsets.h"

using namespace GTASA::SDK;

VersionManager& VersionManager::instance()
{
    static VersionManager inst;
    return inst;
}

bool VersionManager::initialize()
{
    uintptr_t baseAddr = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
    if (!baseAddr)
    {
        LOG_ERROR("[VersionManager] Failed to get module base address!");
        return false;
    }

    // Read Timestamp from PE Header of executable file
    PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(baseAddr);
    PIMAGE_NT_HEADERS ntHeaders =
        reinterpret_cast<PIMAGE_NT_HEADERS>(baseAddr + dosHeader->e_lfanew);
    DWORD timestamp = ntHeaders->FileHeader.TimeDateStamp;

    // Detect version 1.0
    if (checkV1_0(timestamp))
    {
        m_version = GameVersion::V1_0_US_HOODLUM;
        LOG_INFO("[VersionManager] Detected Game Version: GTA SA v1.0 (US Hoodlum)");
        setupV1_0();
        return true;
    }
    // Detect RGL version
    else if (checkRGL(timestamp))
    {
        m_version = GameVersion::RGL_1_0_0_22;
        LOG_INFO("[VersionManager] Detected Game Version: GTA SA RGL (1.0.0.22)");
        setupRGL();
        return true;
    }

    // If the version is unknown, we stop the SDK to prevent a crash!
    LOG_ERROR(
        "[VersionManager] UNKNOWN GAME VERSION! (Timestamp: 0x%X). Halting SDK initialization.",
        timestamp);
    return false;
}

bool VersionManager::checkV1_0(uint32_t timestamp)
{
    // 0x427D3E2F = GTA SA 1.0 US (Hoodlum)
    // 0x427101CA = GTA SA 1.0 (Original or Compact)
    return (timestamp == 0x427D3E2F || timestamp == 0x427101CA);
}

bool VersionManager::checkRGL(uint32_t timestamp)
{
    return timestamp == 0x5d66ae66;
}

// =========================================================================
// Clean initialization using C++20 Designated Initializers
// =========================================================================

void VersionManager::setupV1_0()
{
    Offsets::Dynamic = GameOffsets{.LocalPlayer = 0x77CD98,
                                   .sendHelpMessageFunc = 0x188BE0,
                                   .IdleLoop = 0x219B60,
                                   .OnWanted = 0x161C90,
                                   .OnCrimeCommited = 0x162120,
                                   .SetResolution = 0x3F6CB0};
}

void VersionManager::setupRGL()
{
    Offsets::Dynamic = GameOffsets{.LocalPlayer = 0x80F0D0,
                                   .sendHelpMessageFunc = 0x1946E0,
                                   .IdleLoop = 0x14E080,
                                   .OnWanted = 0x17A830,
                                   .OnCrimeCommited = 0x17AC80,
                                   .SetResolution = 0x42ACB0};
}