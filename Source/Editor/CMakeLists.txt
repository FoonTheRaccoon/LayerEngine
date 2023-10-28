#################################################################################################################################################
# Layer Engine Editor Executable
#################################################################################################################################################

# Add icons (TODO: Add Linux Icon Loading)
set(ICON_SOURCE)
include(${CMAKE_SOURCE_DIR}/CMake/AddIconToBinary.cmake)
AddIconToBinary(ICON_SOURCE ICONS ${CMAKE_SOURCE_DIR}/EditorResources/Textures/Icon/EditorIcon.ico ${CMAKE_SOURCE_DIR}/EditorResources/Textures/Icon/EditorIcon.icns)

# Build executable based on platform
if(WIN32)
	add_executable(LayerEditor WIN32 main.cpp ${ICON_SOURCE})
elseif(APPLE)
    add_executable(LayerEditor MACOSX_BUNDLE main.cpp ${ICON_SOURCE})
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

# Link Editor to the Engine
target_link_libraries(LayerEditor LayerEngine)

# Editor Includes
target_include_directories(LayerEditor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    Core
    EditorUtilities
)

# Editor Source and Headers
target_sources(LayerEditor PRIVATE
"_cpp/Editor.cpp"

PUBLIC # Headers
"Core/Editor.h"

"EditorUtilities/Helpers/EditorFileManager.h"
"EditorUtilities/EditorUtils.h"

"_ThirdParty/EditorThirdParty.h"
"_ThirdParty/EditorThirdPartyConfig.h"
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




