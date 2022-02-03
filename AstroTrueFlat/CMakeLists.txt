# CMakeList.txt : CMake project for AstroTrueFlat, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
enable_language(C CXX ASM_NASM)
set(CMAKE_CXX_STANDARD 20)

find_package(minhook CONFIG REQUIRED)

add_library (AstroTrueFlat SHARED "AstroTrueFlat.cpp" "AstroTrueFlat.h" "hook/Hook.h" "hook/Hook.cpp" "Gui.h" "Gui.cpp" "patch/deformtool_move.asm")
target_link_libraries(AstroTrueFlat minhook::minhook IMGUI d3d11.lib)