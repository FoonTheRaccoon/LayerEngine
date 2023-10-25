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


# Clang specific stuff
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nullability-completeness -Wno-format-security -Wno-unused-variable -Wno-unused-private-field" )
endif()


#Send executables, .dll/.so's, and .lib's to top level Bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin/Lib")

add_subdirectory("Source/Engine")
add_subdirectory("Source/Editor")