#pragma once
#include <cstdint>

namespace GTASA::SDK
{

    enum class GameVersion
    {
        UNKNOWN,
        V1_0_US_HOODLUM,
        RGL_1_0_0_22
    };

    class VersionManager
    {
    public:
        static VersionManager& instance();

        // Main function called at startup
        bool initialize();

        GameVersion getVersion() const
        {
            return m_version;
        }

    private:
        VersionManager() = default;
        ~VersionManager() = default;
        VersionManager(const VersionManager&) = delete;
        VersionManager& operator=(const VersionManager&) = delete;

        GameVersion m_version = GameVersion::UNKNOWN;

        // Helper functions for version detection
        bool checkV1_0(uint32_t timestamp);
        bool checkRGL(uint32_t timestamp);

        // Initialization functions
        void setupV1_0();
        void setupRGL();
    };

} // namespace GTASA::SDK