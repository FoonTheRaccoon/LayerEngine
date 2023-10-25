#################################################################################################################################################
# Layer Engine Editor Executable
#################################################################################################################################################
set(EDITOR_EXE "LayerEditor")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EDITOR_EXE "LayerEditor_Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(EDITOR_EXE "LayerEditor_RelWithDebInfo")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(EDITOR_EXE "LayerEditor_MinSizeRel")
endif()

#if(WIN32)
#	add_executable(${EDITOR_EXE} WIN32 main.cpp)
#else(WIN32)
    add_executable(${EDITOR_EXE} main.cpp)
#endif(WIN32)


# Includes
#target_include_directories(${EDITOR_EXE} PUBLIC
#    
#)

# Header
#target_sources(${EDITOR_EXE} PUBLIC 
#
#)

# Source
#target_sources(${EDITOR_EXE} PRIVATE 
#    
#)


target_link_libraries(${EDITOR_EXE} LayerEngine)