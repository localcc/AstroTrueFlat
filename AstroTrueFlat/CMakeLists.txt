# CMakeList.txt : CMake project for AstroTrueFlat, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
enable_language(C CXX ASM_NASM)

find_path(DETOURS_INCLUDE detours/detours.h)
include_directories(${DETOURS_INCLUDE})
find_library(DETOURS_LIB detours.lib)

add_library (AstroTrueFlat SHARED "AstroTrueFlat.cpp" "AstroTrueFlat.h" "hook/Hook.h" "hook/Hook.cpp" "Gui.h" "Gui.cpp" "patch/deformtool_move.asm")
target_link_libraries(AstroTrueFlat IMGUI ${DETOURS_LIB} d3d11.lib)