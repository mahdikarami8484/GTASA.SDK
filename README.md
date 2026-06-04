# GTASA.SDK

GTASA.SDK is an in-development C++20 SDK for Grand Theft Auto: San Andreas. The goal is to grow it into a complete, developer-friendly SDK for exploring, extending, and interacting with GTA:SA internals from native C++ code. At its current stage, it builds a 32-bit Windows DLL that runs inside the game process, initializes an SDK runtime, installs function hooks, dispatches events, and exposes early wrappers around selected game systems such as the local player, ped, matrix, HUD messages, money, health, armor, and wanted level data.

> This project is intended for learning reverse engineering, game internals, native hooking, and memory-structure exploration. Use it only in a legal, offline, and educational environment that you control.

## What It Does

- Injects/loads as a DLL and starts the SDK runtime from `DllMain`.
- Uses Microsoft Detours to hook selected game, Win32, and Direct3D 9 functions.
- Converts hook callbacks into SDK events through a central event bus.
- Lets scripts subscribe to runtime events and change behavior without manually wiring each script.
- Provides early wrappers for common GTA:SA memory structures and function addresses.
- Includes example scripts for player manipulation, windowed mode tweaks, and process affinity.

## Project Layout

```text
.
|-- CMakeLists.txt              # Main CMake build definition
|-- CMakePresets.json           # Visual Studio 2022 Win32 configure preset
|-- libs/
|   `-- Detours/                # Microsoft Detours dependency as a git submodule
|-- scripts/                    # User scripts that react to SDK events
|   |-- affinity_script.cpp     # Sets process affinity after initialization
|   |-- sample_script.cpp       # Example player/HUD/gameplay script
|   `-- windowed_script.cpp     # Adjusts Win32 and D3D9 parameters for windowed mode
`-- src/
    |-- dllmain.cpp             # DLL entry point; starts and shuts down the SDK runtime
    |-- core/
    |   |-- event/              # BaseEvent and EventBus
    |   |-- game/               # GTA:SA wrappers, offsets, entities, HUD helpers
    |   |-- hook/               # Hook interfaces, registry, manager, auto-registration
    |   |-- logging/            # Debug console logger and circular buffer
    |   |-- math/               # Small math types such as Vector3
    |   |-- script/             # Script base class and auto-registration helper
    |   `-- sdk_runtime.*       # Runtime initialization, hook install, event startup/shutdown
    `-- hooks/                  # Concrete hook implementations
        |-- d3d9_hook.*         # Hooks IDirect3D9::CreateDevice
        |-- game_loop_hook.*    # Hooks selected GTA:SA frame/system functions
        |-- player_hook.*       # Experimental player/crime related hooks
        `-- windows_hook.*      # Hooks CreateWindowExA
```

## Runtime Flow

1. Windows loads the built DLL into the GTA:SA process.
2. `src/dllmain.cpp` creates a separate SDK thread on `DLL_PROCESS_ATTACH`.
3. `SDKRuntime::init()` starts the debug logger and initializes `GameBase` from the current process module.
4. Scripts registered through `AutoRegisterScript<T>` receive an `InitializeEvent`.
5. Hooks registered through `AutoHook<T>` are sorted by priority and installed.
6. `HookManager` enables all Detours hooks in one transaction.
7. Hook callbacks create events such as `ProcessFrameEvent`, `CreateWindowExAEvent`, and `CreateDeviceEvent`.
8. `EventBus` dispatches those events to enabled scripts.
9. On unload, `SDKRuntime::shutdown()` disables hooks, sends `ShutdownEvent`, and stops logging.

## Requirements

- Windows
- Visual Studio 2022 with the C++ desktop workload
- CMake 3.20 or newer
- Git with submodule support
- Microsoft DirectX SDK (June 2010)
  - The current `CMakeLists.txt` expects headers at:
    `C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include`
- A GTA:SA 1.0 US-style executable/layout compatible with the offsets used in `src/core/game/offsets.h` and the hook addresses in `src/hooks/`

Because GTA:SA is a 32-bit game, the project must be built as Win32/x86. The included CMake preset already targets `Win32`.

## Clone

Clone the repository with submodules:

```powershell
git clone --recursive https://github.com/mahdikarami8484/GTASA.SDK.git
cd GTASA.SDK
```

If you already cloned without `--recursive`, initialize the Detours submodule manually:

```powershell
git submodule update --init --recursive
```

## Configure The Game Output Path

The build currently copies the generated DLL into the GTA:SA directory as `samp.dll` after every successful build:

```cmake
set(GTA_SA_PATH "E:/Grand Theft Auto - San Andreas - Copy/")

add_custom_command(TARGET GTASA_SDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:GTASA_SDK>
        "${GTA_SA_PATH}/samp.dll"
)
```

Before building, edit `CMakeLists.txt` and change `GTA_SA_PATH` to your local GTA:SA folder. If you do not want automatic deployment, remove or comment out the `add_custom_command` block and copy the DLL manually from the build output directory.

## Build With CMake

From a Visual Studio Developer PowerShell or a terminal where MSVC tools are available:

```powershell
cmake --preset msvc-x86
cmake --build build --config Debug
```

The preset uses:

- Generator: `Visual Studio 17 2022`
- Architecture: `Win32`
- Build directory: `build/`
- C++ standard: C++20

## Build With Visual Studio

1. Clone the repository and initialize submodules.
2. Open the repository folder in Visual Studio 2022 and let Visual Studio configure the project using the included CMake preset (`CMakePresets.json`).
3. Select a Win32/x86 configuration.
4. Make sure the DirectX SDK include path and Detours submodule are available.
5. Build the `GTASA_SDK` target.

## Running And Testing

Once the build finishes successfully, you can run and test the SDK using the custom launcher:

1. Navigate to the build output directory (e.g., `out/Debug/` or wherever your compiled launcher executable is located).
2. Run the `launcher.exe` file.
3. A file dialog or prompt will appear; use it to select your Grand Theft Auto: San Andreas executable (`gta_sa.exe`).
4. The launcher will automatically boot the game and inject the `GTASA_SDK.dll` into the game process at startup.
5. You can now test your scripts, view the debug logging console (in Debug configuration), and experiment with the SDK features live in-game!

## Creating A Script

Scripts inherit from `GTASA::SDK::Script` and handle events through `onEvent`. Register a script by creating a static `AutoRegisterScript<T>` instance:

```cpp
#include "core/script/script.h"
#include "core/script/auto_register_script.h"
#include "core/sdk_runtime.h"

namespace GTASA::SDK {

class MyScript : public Script {
public:
    void onEvent(std::shared_ptr<BaseEvent> event) override {
        if (dynamic_cast<const Events::InitializeEvent*>(event.get())) {
            // Initialize script state here.
        }
    }
};

}

namespace {
    GTASA::SDK::AutoRegisterScript<GTASA::SDK::MyScript> _autoReg;
}
```

Place new script files under `scripts/`. CMake automatically includes `scripts/*.cpp` through `file(GLOB_RECURSE ...)`.

## Notes And Safety

- Addresses and offsets are version-specific. If your GTA:SA executable differs, hooks or memory reads may crash.
- Debug builds allocate a console for SDK logs; release builds skip the logger console path through `NDEBUG`.
- Some hooks are experimental and may be incomplete or disabled in code.
- Keep reverse engineering work educational, local, and respectful of software licenses and online service rules.

## License

GTASA.SDK is licensed under the GNU General Public License, version 3
only (`GPL-3.0-only`).

If you convey or distribute this program, or modified or derivative
versions of it, those distributed works must be licensed under GPLv3
and the corresponding source must be made available under the license
terms.

See the LICENSE file for the full license text.
