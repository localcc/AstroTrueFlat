# AstroTrueFlat

This mod allows "true flat" surfaces to be created anywhere in the world.

## Install

1. Download the latest dll from [Releases](https://github.com/localcc/AstroTrueFlat/releases/latest)
2. Find a dll injector of your choice, e.g. process hacker
3. Inject the dlls inside `Astro-Win64-Shipping.exe` / `Astro-UWP64-Shipping.exe`
    * Note for UWP users: Open dll file properties, security tab, edit, and add `all` with `Read&Execute` permissions.
    * This needs to be done for all generated dlls once.
4. If you don't see the menu - press insert

## Compiling

### Prerequisites

* Clang 13.0.0 or higher
* CMake
* Visual Studio 2022 or higher
* vcpkg
* minhook installed in vcpkg
* git

### Building
* Win32
    1. Clone the project and all of it's submodules
    2. Open the project in Visual Studio and configure `CMakeSettings.json` to use clang and vcpkg
    3. Build the project
* UWP
    1. Clone the project and all of it's submodules
    2. Generate project makefiles with `Visual Studio 17 2022` generator, x64 architecture, vcpkg, clang and `-DUWP=ON` option.
    3. Open the project in visual studio
    4. Build `ALL_BUILD` project
