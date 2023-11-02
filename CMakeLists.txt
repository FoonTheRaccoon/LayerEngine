#################################################################################################################################################
# Layer Engine Project Entry
#################################################################################################################################################
cmake_minimum_required(VERSION 3.20.0)
project(
	"Layer"
	VERSION 1.0.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Recommended non MSVC Windows toolchain: msys2 mingw-w64-clang-x86_64-toolchain (Clang, LLD) + Ninja + ccache

# Check for recommended toolchain if not MSVC
if(NOT MSVC)
	# Set the generator to Ninja if it's available
	find_program(NINJA_EXE ninja)
	if(NINJA_EXE)
		set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)
		message(STATUS "Ninja is available, using Ninja as the generator")
	else()
		message(STATUS "Ninja is not available, using the default generator")
	endif()

	# Use lld linker if available
	find_program(LLD_EXE lld)
	if(LLD_EXE)
		add_compile_options(-fuse-ld=lld)
		message(STATUS "LLD is available, using LLD as the linker")
	else()
		message(STATUS "LLD is not available, using the default linker")
	endif()

	# Use ccache if available
	find_program(CCACHE_EXE ccache)
	if(CCACHE_EXE)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_EXE}")
		message(STATUS "Using CCache")
	else()
		message(STATUS "CCache not available")
	endif()
endif(NOT MSVC)

# Set avx2 and any other compiler specific options
if(MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		add_compile_options(/O2 /arch:AVX2)
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_options(/Zi /Od)
	elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		add_compile_options(/O2 /Zi /arch:AVX2)
	elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
		add_compile_options(/Os /arch:AVX2)
	endif()
elseif(ARMCC OR ARMClang)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		add_compile_options(-O3 -fvectorize)
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_options(-g -O0)
	elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		add_compile_options(-O2 -g -fvectorize)
	elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
		add_compile_options(-Os -fvectorize)
	endif()
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		add_compile_options(-O3 -mavx2)
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_options(-g -O0)
	elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		add_compile_options(-O2 -g -mavx2)
	elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
		add_compile_options(-Os -mavx2)
	endif()
endif()

# Clang specific stuff
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(-ftime-trace
	-Wno-reserved-identifier
	-Wno-nullability-completeness
	-Wno-unused-variable
	-Wno-unused-private-field
	)
endif()

# Send executables, .dll/.so's, and .lib's to top level Bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin/Lib")

add_subdirectory("Source/Engine")
add_subdirectory("Source/Editor")