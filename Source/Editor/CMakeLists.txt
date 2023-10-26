#################################################################################################################################################
# Layer Engine Editor Executable
#################################################################################################################################################

# Add icons (TODO: Add Linux Icon Loading)
set(ICON_SOURCE)
include(${CMAKE_SOURCE_DIR}/CMake/AddIconToBinary.cmake)
AddIconToBinary(ICON_SOURCE ICONS ${CMAKE_SOURCE_DIR}/EditorResources/Textures/Icon/EditorIcon.ico ${CMAKE_SOURCE_DIR}/EditorResources/Textures/Icon/EditorIcon.icns)

# Build executable based on platform
if(WIN32)
	add_executable(LayerEditor WIN32 main.cpp)
elseif(APPLE)
    add_executable(LayerEditor MACOSX_BUNDLE main.cpp)
else()
    add_executable(LayerEditor main.cpp)
endif()

# Set which Engine features to enable
target_compile_definitions(LayerEngine PUBLIC
    LAYER_EDITOR
    LAYER_USE_LIVE_LOGGER
    LAYER_USE_VERBOSE_LOGGER
    LAYER_USE_VALIDATION_LAYERS
    LAYER_DEBUG
)

# Link to the Engine
target_link_libraries(LayerEditor LayerEngine)

# Includes
target_include_directories(LayerEditor PUBLIC
    Header
    Header/EditorUtilities
)

# Editor Headers
file(GLOB_RECURSE EDITOR_HEADER *.h)
target_sources(LayerEditor PUBLIC ${EDITOR_HEADER})

# Editor Source
file(GLOB_RECURSE EDITOR_SOURCE *.cpp)
target_sources(LayerEditor PUBLIC ${EDITOR_SOURCE} ${ICON_SOURCE})



#################################################################################################################################################
# Layer Engine Editor Third Party
#################################################################################################################################################

# Editor Third Party Includes
target_include_directories(LayerEditor PUBLIC
    ThirdParty
    ThirdParty/assimp
    ThirdParty/stb
)

# ASSIMP (Static)


# STB (Source)




