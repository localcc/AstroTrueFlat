# AstroTrueFlat

This mod allows "true flat" surfaces to be created anywhere in the world.

## Install

1. Download the latest dll from [Releases](https://github.com/localcc/AstroTrueFlat/releases/latest)
2. Find a dll injector of your choice, e.g. process hacker
3. Inject the dll inside `Astro-Win64-Shipping.exe`
4. If you don't see the menu - press insert

## Compiling

### Prerequisites

* Clang 13.0.0 or higher
* CMake
* Visual Studio 2022 or higher
* vcpkg
* detours installed in vcpkg
* git

### Building
1. Clone the project and all of it's submodules
2. Open the project in Visual Studio and configure `CMakeSettings.json` to use clang and vcpkg
3. Build the project
