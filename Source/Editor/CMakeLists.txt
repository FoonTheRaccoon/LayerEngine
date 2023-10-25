#################################################################################################################################################
# Layer Engine Editor Executable
#################################################################################################################################################

#if(CMAKE_BUILD_TYPE STREQUAL "Release")
#    add_compile_options(-O3)
#elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    add_compile_options(-g -O0)
#elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
#    add_compile_options(-O2 -g)
#elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
#    add_compile_options(-Os)
#endif()

#if(WIN32)
#	add_executable(LayerEditor WIN32 main.cpp)
#else(WIN32)
    add_executable(LayerEditor main.cpp)
#endif(WIN32)

target_compile_definitions(LayerEditor PUBLIC
    LAYER_EDITOR
    LAYER_USE_LIVE_LOGGER
    LAYER_USE_VERBOSE_LOGGER
    LAYER_USE_VALIDATION_LAYERS
    LAYER_DEBUG
)


# Includes
#target_include_directories(LayerEditor PUBLIC
#    
#)

# Header
#target_sources(LayerEditor PUBLIC 
#
#)

# Source
#target_sources(LayerEditorPRIVATE 
#    
#)


target_link_libraries(LayerEditor LayerEngine)