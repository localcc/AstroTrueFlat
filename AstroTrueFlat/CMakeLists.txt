# CMakeList.txt : CMake project for AstroTrueFlat, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
enable_language(C CXX ASM_NASM)
set(CMAKE_CXX_STANDARD 20)

option(UWP "Compile for UWP version of the game" OFF)

find_package(minhook CONFIG REQUIRED)

add_library (AstroTrueFlat SHARED "AstroTrueFlat.cpp" "AstroTrueFlat.h" "hook/Hook.h" "hook/Hook.cpp" "Gui.h" "Gui.cpp" "patch/deformtool_move.asm" "patch/deformtool_move_uwp.asm")
target_link_libraries(AstroTrueFlat PRIVATE minhook::minhook IMGUI d3d11.lib)

if(UWP)
	add_compile_definitions(UWP)
	# https://github.com/Microsoft/cppwinrt/issues/124
	find_program(NUGET_EXE NAMES nuget)
	if(NOT NUGET_EXE)
		message("NUGET.EXE not found.")
		message(FATAL_ERROR "Please install this executable, and run CMake again.")
	endif()

	exec_program(${NUGET_EXE}
		ARGS install "Microsoft.Windows.CppWinRT" -Version 2.0.220131.2 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)

	set_target_properties(AstroTrueFlat PROPERTIES
	  VS_PROJECT_IMPORT ${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.CppWinRT/build/native/Microsoft.Windows.CppWinRT.props)

	target_link_libraries(AstroTrueFlat PRIVATE ${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.CppWinRT/build/native/Microsoft.Windows.CppWinRT.targets)

endif(UWP)