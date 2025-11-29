# OpenHD Developer Guide

This guide is designed to help new developers understand and contribute to the OpenHD codebase.

## Table of Contents
- [Project Overview](#project-overview)
- [Prerequisites](#prerequisites)
- [Repository Structure](#repository-structure)
- [Building the Project](#building-the-project)
- [Architecture Overview](#architecture-overview)
- [Module Deep Dive](#module-deep-dive)
- [Code Patterns and Conventions](#code-patterns-and-conventions)
- [Adding New Features](#adding-new-features)
- [Testing](#testing)
- [Debugging](#debugging)

---

## Project Overview

OpenHD is an open-source digital video transmission system designed for FPV (First Person View) applications, primarily used in drones. It transmits low-latency HD video and telemetry data over WiFi in monitor mode.

The system consists of two units:
- **Air Unit**: Attached to the drone, captures video from cameras and transmits it
- **Ground Unit**: Receives the video stream and displays it to the pilot

### Key Technologies
- C++17
- CMake build system
- GStreamer for video pipeline
- MAVLink for telemetry
- WiFi monitor mode (wifibroadcast)

---

## Prerequisites

### Development Environment
- Linux (Ubuntu 20.04+ recommended)
- GCC 8+ or Clang 10+
- CMake 3.16.3+
- Git

### Required Dependencies
```bash
# Core build tools
sudo apt install build-essential cmake git

# GStreamer (for video)
sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
                 gstreamer1.0-plugins-good gstreamer1.0-plugins-bad

# Optional: libcamera support (for Raspberry Pi cameras)
sudo apt install libcamera-dev

# Optional: USB camera support
sudo apt install libusb-1.0-0-dev libv4l-dev
```

### Submodules
The project uses Git submodules. Initialize them with:
```bash
git submodule update --init --recursive
```

---

## Repository Structure

```
OpenHD/
├── main.cpp                 # Application entry point
├── CMakeLists.txt           # Main CMake configuration
├── build_cmake.sh           # Build script
│
├── ohd_common/              # Shared utilities and types
│   ├── inc/                 # Headers
│   │   ├── openhd_platform.h    # Platform detection
│   │   ├── openhd_spdlog.h      # Logging utilities
│   │   └── ...
│   ├── src/                 # Implementation files
│   └── lib/                 # External libraries (spdlog, json)
│
├── ohd_interface/           # WiFi/wifibroadcast management
│   ├── inc/
│   │   ├── wifi_card.h          # WiFi card abstraction
│   │   ├── wifi_card_discovery.cpp  # Card detection
│   │   └── wb_link.h            # Wifibroadcast link
│   └── src/
│
├── ohd_video/               # Video capture and streaming
│   ├── inc/
│   │   ├── camera.hpp           # Camera definitions and registry
│   │   ├── camera_info.hpp      # CameraInfo class hierarchy
│   │   └── gst_helper.hpp       # GStreamer pipeline helpers
│   └── src/
│
└── ohd_telemetry/           # MAVLink telemetry handling
    ├── inc/
    └── src/
```

---

## Building the Project

### Quick Build
```bash
cd OpenHD
./build_cmake.sh
```

### Manual Build
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Build Options
| Option | Description | Default |
|--------|-------------|---------|
| `ENABLE_AIR` | Enable air unit support (video capture) | ON |
| `ENABLE_USB_CAMERAS` | Enable USB camera support | ON |

Example with options:
```bash
cmake -DENABLE_USB_CAMERAS=OFF ..
```

### Running
```bash
# Run as air unit (on drone)
sudo ./build/openhd --air

# Run as ground unit
sudo ./build/openhd --ground

# Clean start (reset all settings)
sudo ./build/openhd --ground --clean-start
```

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                        main.cpp                              │
│  - Parses command line arguments                            │
│  - Initializes all modules                                  │
│  - Coordinates startup/shutdown                             │
└─────────────────────────────────────────────────────────────┘
                              │
     ┌────────────────────────┼────────────────────────┐
     ▼                        ▼                        ▼
┌─────────────┐      ┌─────────────────┐      ┌─────────────┐
│OHDInterface │      │  OHDTelemetry   │      │  OHDVideo   │
│             │      │                 │      │ (Air/Ground)│
│- WiFi cards │◄────►│- MAVLink        │◄────►│- Cameras    │
│- WB link    │      │- Settings       │      │- GStreamer  │
│- Encryption │      │- FC connection  │      │- Recording  │
└─────────────┘      └─────────────────┘      └─────────────┘
     │                        │                        │
     └────────────────────────┼────────────────────────┘
                              ▼
                    ┌─────────────────┐
                    │   OHDCommon     │
                    │                 │
                    │- Platform detect│
                    │- Logging        │
                    │- Config files   │
                    │- Utilities      │
                    └─────────────────┘
```

### Module Communication
Modules communicate through:
1. **LinkActionHandler** - Global singleton for cross-module actions (e.g., bitrate changes)
2. **Settings system** - Shared settings with MAVLink-based configuration
3. **Direct handles** - Modules receive handles to other modules during initialization

---

## Module Deep Dive

### ohd_common
Shared utilities used across all modules:
- **openhd_platform.h/cpp**: Platform detection (RPI, Rockchip, X20, etc.)
- **openhd_spdlog.h**: Logging with spdlog
- **openhd_config.h**: Configuration file handling
- **openhd_util.h**: General utilities

### ohd_interface
WiFi and wifibroadcast management:
- **wifi_card.h**: WiFi card types and capabilities
- **wifi_card_discovery.cpp**: Detects connected WiFi cards
- **wb_link.h**: Wifibroadcast link management
- **ohd_interface.h**: Main interface class

### ohd_video
Video capture and streaming:
- **camera.hpp**: Camera type definitions and registry
- **camera_info.hpp**: `CameraInfo` class hierarchy for camera metadata
- **camera_discovery.cpp**: Camera detection
- **gstreamerstream.cpp**: GStreamer pipeline management
- **ohd_video_air.h**: Air unit video handling
- **ohd_video_ground.h**: Ground unit video reception

### ohd_telemetry
MAVLink telemetry:
- **OHDTelemetry.h**: Main telemetry class
- **OHDMainComponent.cpp**: MAVLink message handling
- **endpoints/**: Various telemetry endpoints (serial, UDP)

---

## Code Patterns and Conventions

### 1. Lookup Table Pattern
We use lookup tables instead of if-else chains for maintainability:

```cpp
// Good: Lookup table
static const std::unordered_map<int, std::string> kPlatformTypeNames = {
    {X_PLATFORM_TYPE_RPI_4, "RPI 4"},
    {X_PLATFORM_TYPE_X86, "X86"},
    // ...
};

std::string get_platform_name(int type) {
    auto it = kPlatformTypeNames.find(type);
    return (it != kPlatformTypeNames.end()) ? it->second : "UNKNOWN";
}

// Avoid: Long if-else chains
```

### 2. Registry Pattern
Used for cameras and will be used for similar extensible components:

```cpp
// camera_info.hpp
class CameraInfo {
public:
    int get_type() const;
    std::string get_name() const;
    CameraFamily get_family() const;
    std::vector<ResolutionFramerate> get_resolutions() const;
    // ...
};

class CameraRegistry {
public:
    static CameraRegistry& instance();
    const CameraInfo* get_camera_info(int type) const;
    // ...
};
```

### 3. Capability Sets
For feature flags, use sets instead of if-else:

```cpp
namespace wifi_card_capabilities {
    static const std::unordered_set<WiFiCardType> kSupportsVariableMcs = {
        WiFiCardType::ATHEROS_AR9271,
        WiFiCardType::REALTEK_RTL8812AU,
        // ...
    };
}

bool supports_variable_mcs(WiFiCardType type) {
    return kSupportsVariableMcs.count(type) > 0;
}
```

### 4. Namespace Organization
Related functionality is grouped in namespaces:
- `openhd::` - General OpenHD utilities
- `resolution_names::` - Resolution display names
- `platform_camera_choices::` - Platform-specific camera options
- `wifi_card_capabilities::` - WiFi card feature sets

### 5. Logging
Use spdlog for logging:
```cpp
#include "openhd_spdlog.h"

auto console = openhd::log::create_or_get("module_name");
console->debug("Debug message");
console->info("Info message");
console->warn("Warning: {}", some_value);
console->error("Error occurred");
```

---

## Adding New Features

### Adding a New Camera
1. Add camera type constant in `camera.hpp`:
```cpp
static constexpr int X_CAM_TYPE_MY_CAMERA = 130;
```

2. Register the camera in `camera_info.cpp`:
```cpp
register_camera(std::make_unique<CameraInfo>(
    X_CAM_TYPE_MY_CAMERA,
    "MY_CAMERA",
    CameraFamily::USB,  // or appropriate family
    CameraPipelineType::USB_GENERIC,
    make_resolutions({{1920, 1080, 30}, {1280, 720, 60}}),
    false  // supports_iq_params
));
```

3. Add platform support in `camera.hpp` `platform_camera_choices` namespace if needed.

### Adding a New Platform
1. Add platform constant in `openhd_platform.h`:
```cpp
static constexpr int X_PLATFORM_TYPE_MY_PLATFORM = 50;
```

2. Add detection logic in `openhd_platform.cpp` `internal_discover_platform()`.

3. Add platform name in `kPlatformTypeNames` map.

4. Add to appropriate category sets in `platform_categories` namespace.

### Adding a New WiFi Card
1. Add card type to `WiFiCardType` enum in `wifi_card.h`.

2. Add driver mapping in `wifi_card_discovery.cpp` `kExactDriverMatches` or `kSubstringDriverMatches`.

3. Add to capability sets as appropriate in `wifi_card_capabilities` namespace.

---

## Testing

### Running Tests
Individual modules have test executables:
```bash
cd build
./ohd_video/test_video
./ohd_interface/test_wifi_commands
```

### Testing Air/Ground Modes
```bash
# Test as air (will use dummy camera if none found)
sudo ./openhd --air --run-time-seconds 60

# Test as ground
sudo ./openhd --ground --run-time-seconds 60
```

---

## Debugging

### Useful Commands
See `usefull_commands_for_debugging.txt` for debugging commands.

### Logging
Logs are output to stdout by default. Use `spdlog` log levels to control verbosity.

### Common Issues

1. **"Must run as root"**: OpenHD requires root for WiFi monitor mode
2. **"No WiFi cards found"**: Ensure cards support monitor mode and drivers are patched
3. **"Camera not detected"**: Check camera connection and driver support

### GStreamer Debugging
```bash
# Enable GStreamer debug output
export GST_DEBUG=3
./openhd --air
```

---

## Code Style

### Formatting
Use the provided `.clang-format` file:
```bash
./run_clang_format.sh
```

### Naming Conventions
- Classes: `PascalCase` (e.g., `CameraInfo`)
- Functions: `snake_case` (e.g., `get_camera_info`)
- Constants: `kPascalCase` or `SCREAMING_SNAKE_CASE` for legacy
- Namespaces: `snake_case` (e.g., `platform_camera_choices`)

---

## Getting Help

- Check existing code for patterns and examples
- Review the README.md files in each module
- Open an issue on GitHub for questions

## License

OpenHD is licensed under GPL v3. See LICENSE file for details.
