#################################################################################################################################################
# Layer Engine Editor Executable
#################################################################################################################################################

set(LAYER_EDITOR_SRC
        # Source
        _cpp/Editor.cpp
        _cpp/EditorFileManager.cpp

        # Headers
        Core/Editor.h

        EditorUtilities/Helpers/EditorFileManager.h
        EditorUtilities/EditorUtils.h

        _ThirdParty/EditorThirdParty.h
        _ThirdParty/EditorThirdPartyConfig.h
)

# Add icons (TODO: Add Linux Icon Loading)
set(ICON_SOURCE)
include(${CMAKE_SOURCE_DIR}/CMake/AddIconToBinary.cmake)
AddIconToBinary(ICON_SOURCE ICONS ${CMAKE_SOURCE_DIR}/EditorResources/Textures/Icon/EditorIcon.ico ${CMAKE_SOURCE_DIR}/EditorResources/Textures/Icon/EditorIcon.icns)

# Build executable based on platform
if(WIN32)
	add_executable(LayerEditor WIN32 main.cpp ${LAYER_EDITOR_SRC} ${ICON_SOURCE})
elseif(APPLE)
    add_executable(LayerEditor MACOSX_BUNDLE main.cpp ${LAYER_EDITOR_SRC} ${ICON_SOURCE})
else()
    add_executable(LayerEditor main.cpp ${LAYER_EDITOR_SRC})
endif()


# Set which Engine features to enable
target_compile_definitions(LayerEngine PUBLIC
    LAYER_EDITOR
    LAYER_USE_UI
    LAYER_USE_LIVE_LOGGER
    LAYER_USE_VERBOSE_LOGGER
    LAYER_USE_VALIDATION_LAYERS
    LAYER_DEBUG
)

# Link Editor to the Engine
target_link_libraries(LayerEditor LayerEngine)

# Editor Includes
target_include_directories(LayerEditor PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        Core
        EditorUtilities
        EditorUtilities/Helpers
)


#################################################################################################################################################
# Layer Engine Editor Third Party
#################################################################################################################################################

# Editor Third Party Includes
target_include_directories(LayerEditor PUBLIC
    _ThirdParty
    _ThirdParty/assimp
    _ThirdParty/stb
)

# ASSIMP (Static)


# STB (Source)




