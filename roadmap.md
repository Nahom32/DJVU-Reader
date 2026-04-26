# DJVU Reader - Architectural Roadmap

## Executive Summary

This roadmap outlines the complete architecture and implementation plan for building a cross-platform DJVU reader application using C++ with Qt5 and libdjvulibre. The application will support Windows, macOS, and Linux with full ergonomic controls.

---

## Issue Analysis

### Current Problem
The existing project has a **fundamental dependency mismatch**:
- `djvu_reader.cpp` is **C++** code using Qt5 + libdjvulibre
- `requirements.txt` contains **Python** packages (`python-djvulibre`, `pillow`, `PyQt5`)

These are incompatible and cannot work together.

### Solution
Remove Python dependencies entirely. Use native C++ libdjvulibre library with Qt5 for a proper cross-platform desktop application.

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    DJVU Reader Application                   │
├─────────────────────────────────────────────────────────────┤
│  UI Layer (Qt5 Widgets)                                    │
│  ├── MainWindow (menu bar, toolbar)                         │
│  ├── DjvuView (scrollable image area)                      │
│  ├── ZoomControls (toolbar buttons)                        │
│  └── PageNavigator (prev/next, slider)                    │
├─────────────────────────────────────────────────────────────┤
│  Core Layer (libdjvulibre)                                 │
│  ├── DocumentController                                    │
│  ├── PageRenderer                                           │
│  └── DjvuContext (async message handling)                  │
├─────────────────────────────────────────────────────────────┤
│  Build System (CMake)                                       │
│  ├── Find_package(Qt5 REQUIRED COMPONENTS Widgets)        │
│  ├── Find_package(djvulibre)                              │
│  └── CPack installers (Windows/macOS/Linux)               │
└─────────────────────────────────────────────────────────────┘
```

---

## Target Platforms

| Platform | Package Type | Installation Method |
|----------|--------------|---------------------|
| Windows | `.exe` (NSIS installer) | Double-click installer |
| macOS | `.dmg` (app bundle) | Drag to Applications |
| Linux | `.deb`, `.rpm` | Package manager |

---

## File Structure

```
DJVU-Reader/
├── roadmap.md                    # This document
├── CMakeLists.txt               # Build configuration
├── src/
│   ├── main.cpp                 # Entry point
│   ├── mainwindow.cpp           # Main window with toolbar
│   ├── mainwindow.h
│   ├── mainwindow.ui            # Qt Designer UI
│   ├── djvuview.cpp             # Custom scrollable DJVU viewer
│   ├── djvuview.h
│   ├── pagerenderer.cpp        # DJVU → QImage conversion
│   └── pagerenderer.h
├── resources/
│   └── icons/                  # Application icons
└── config/
    └── app.ini                 # App configuration
```

---

## Dependencies

### Required Libraries

| Library | Purpose | Platform Notes |
|---------|---------|----------------|
| Qt5 Widgets | GUI framework | Install via package manager or Qt installer |
| libdjvulibre | DJVU file format | C++ API for decoding DJVU files |
| cmake | Build system | Version 3.16+ |
| C++ compiler | Compilation | g++ (Unix), MSVC or MinGW (Windows) |

### Installation Commands

**macOS:**
```bash
brew install qt5 djvulibre
```

**Linux (Debian/Ubuntu):**
```bash
sudo apt update
sudo apt install libdjvulibre-dev qtbase5-dev cmake build-essential
```

**Linux (Fedora/RHEL):**
```bash
sudo dnf install djvulibre-devel qt5-devel cmake
```

**Windows:**
```powershell
# Using vcpkg
vcpkg integrate install
vcpkg install djvulibre:x64-windows qt5:x64-windows
```

Or download pre-built binaries from: https://djvulibre.gitlab.io/

---

## UI Components Specification

### Main Window Features

| Component | Description | Keyboard Shortcut |
|-----------|-------------|-------------------|
| File Open | Open DJVU file | Ctrl+O |
| Close | Close current document | Ctrl+Q |
| Exit | Exit application | Alt+F4 |
| Zoom In | Increase zoom level | Ctrl++ and Ctrl+MouseWheel Up |
| Zoom Out | Decrease zoom level | Ctrl+- and Ctrl+MouseWheel Down |
| Fit Width | Fit page to window width | Ctrl+9 |
| Fit Page | Fit entire page in window | Ctrl+0 |
| Actual Size | 100% zoom | Ctrl+1 |
| Previous Page | Go to previous page | PageUp or Left Arrow |
| Next Page | Go to next page | PageDown or Right Arrow |
| Go to Page | Jump to specific page | Ctrl+G |

### Toolbar Layout

```
[Open] [Close] | [Zoom -][100%][Zoom +] | [Fit Width][Fit Page] | [|<][<][Page ][>][>|]
```

### Zoom Controls

- **Zoom Range**: 10% to 400%
- **Zoom Steps**: 10%, 25%, 50%, 75%, 100%, 150%, 200%, 300%, 400%
- **Default Zoom**: Fit to window width
- **Mouse Wheel**: Ctrl+Wheel for zoom in/out

### Page Navigation

- **Display**: Current page / Total pages
- **Slider**: Continuous page navigation
- **Buttons**: First, Previous, Next, Last page
- **Keyboard**: Arrow keys, Page Up/Down, Home/End

---

## Build System Configuration

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(Djvureader VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Find libdjvulibre
find_package(djvulibre QUIET)
if(NOT djvulibre_FOUND)
    message(WARNING "libdjvulibre not found, building internal version")
    # Add subdirectory for bundled libdjvulibre
endif()

# Application sources
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/djvuview.cpp
    src/pagerenderer.cpp
)

add_executable(djvureader ${SOURCES})

target_link_libraries(djvureader
    Qt5::Widgets
    Qt5::Gui
)

# Installation
install(TARGETS djvureader
    RUNTIME DESTINATION bin
)

# CPack configuration
include(CPack)
```

### CPack Configuration

```cmake
set(CPACK_PACKAGE_NAME "DJVU Reader")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "DJVU Reader Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform DJVU document viewer")

# Windows NSIS installer
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_INSTALLER_ICON "${CMAKE_SOURCE_DIR}/resources/icons/app.ico")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/icons/app.ico")

# macOS DMG
set(CPACK_OSX_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/resources/icons/app.icns")
set(CPACK_OSX_BUNDLE_GUI_IDENTIFIER "com.djvureader.app")

# Linux packages
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= 5.0), libdjvulibre5 (>= 3.5)")
set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase, djvulibre")

include(CPack)
```

---

## Packaging and Distribution

### Build Commands

```bash
# Configure with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the application
cmake --build build

# Package for current platform
cpack --config-file build/cpack-config.cmake
```

### Platform-Specific Packages

| Platform | Generate Package | Output File |
|----------|---------------|------------|
| Windows | `cpack -G NSIS` | `DJVU-Reader-1.0.0-setup.exe` |
| macOS | `cpack -G DragNDrop` | `DJVU-Reader-1.0.0.dmg` |
| Linux (DEB) | `cpack -G DEB` | `djvureader_1.0.0_amd64.deb` |
| Linux (RPM) | `cpack -G RPM` | `djvureader-1.0.0.x86_64.rpm` |

---

## Implementation Phases

### Phase 1: Project Setup (Priority: High)
- [ ] Create CMakeLists.txt
- [ ] Set up directory structure
- [ ] Configure Qt5 and libdjvulibre finding
- [ ] Verify empty shell builds

### Phase 2: Core DJVU Rendering (Priority: High)
- [ ] Implement DjvuContext wrapper
- [ ] Implement PageRenderer class
- [ ] Create basic document loading
- [ ] Single page rendering to QImage

### Phase 3: Main Window (Priority: High)
- [ ] Create MainWindow with menu bar
- [ ] Implement file open dialog
- [ ] Add status bar with page info
- [ ] Connect core rendering to UI

### Phase 4: Zoom Controls (Priority: Medium)
- [ ] Add zoom toolbar buttons
- [ ] Implement zoom logic (10%-400%)
- [ ] Add keyboard shortcuts
- [ ] Mouse wheel zoom support

### Phase 5: Page Navigation (Priority: Medium)
- [ ] Add page slider
- [ ] Add prev/next buttons
- [ ] Implement multi-page support
- [ ] Keyboard navigation

### Phase 6: Ergonomic Features (Priority: Medium)
- [ ] Close button in toolbar
- [ ] Recent files menu
- [ ] Window state persistence
- [ ] Remember last position

### Phase 7: Packaging (Priority: High)
- [ ] Configure CPack for all platforms
- [ ] Create application icons
- [ ] Test installation on each platform
- [ ] Generate platform installers

---

## Alternative Approaches

### Option 1: Build on Existing Djview4
- **Pros**: Feature-complete, well-tested, actively maintained
- **Cons**: Qt4/Qt5 hybrid, less control over UI
- Reference: https://github.com/barak/djview4

### Option 2: Use ddjvuapi with Custom UI (Current Plan)
- **Pros**: Full control, modern Qt5, clean implementation
- **Cons**: More implementation work
- This roadmap follows this approach

### Option 3: WebAssembly with DJVU.js
- **Pros**: Truly cross-platform (browser-based)
- **Cons**: Performance limitations, no native features
- Reference: https://github.com/nickspirit/djvu.js

---

## References

- libdjvulibre: http://djvu.sourceforge.net/
- Qt5 Documentation: https://doc.qt.io/qt-5/
- CMake Tutorial: https://cmake.org/cmake/help/latest/
- CPack Documentation: https://cmake.org/cmake/help/latest/module/CPack.html

---

## Summary

1. **Remove** Python dependencies (`requirements.txt`)
2. **Create** CMake build system with Qt5 + libdjvulibre
3. **Implement** MainWindow with toolbar and zoom controls
4. **Add** PageRenderer for DJVU → QImage conversion
5. **Configure** CPack for Windows/macOS/Linux installers
6. **Test** packaging on all target platforms

Following this roadmap will produce a fully functional, cross-platform DJVU reader with proper ergonomic controls for document viewing.