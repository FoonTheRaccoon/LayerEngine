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
	#-Wno-nullability-completeness
	#-Wno-unused-variable 
	#-Wno-unused-private-field
	)
endif()

# Benchmark Compile Time
#set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

# Send executables, .dll/.so's, and .lib's to top level Bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin/Lib")

add_subdirectory("Source/Engine")
add_subdirectory("Source/Editor")