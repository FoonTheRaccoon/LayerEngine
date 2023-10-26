#################################################################################################################################################
# Layer Engine Library
#################################################################################################################################################
add_library(LayerEngine STATIC)
set_target_properties(LayerEngine PROPERTIES LINKER_LANGUAGE CXX)

# Engine Headers
file(GLOB_RECURSE ENGINE_HEADER Header/*.h)
target_sources(LayerEngine PUBLIC ${ENGINE_HEADER})

# Engine Source
file(GLOB_RECURSE ENGINE_SOURCE Source/*.cpp)
target_sources(LayerEngine PUBLIC ${ENGINE_SOURCE})


# Engine Includes
target_include_directories(LayerEngine PUBLIC
    Header
    Header/Components
    Header/Data
    Header/Entities
    Header/Managers
    Header/Render
    Header/Render/Vulkan
    Header/Utilities
    Header/Utilities/Events
    Header/Utilities/Helpers
    Header/Utilities/Logger
    Header/Utilities/Memory
    Header/Utilities/Types
    Header/Utilities/UI
)



#################################################################################################################################################
# Layer Engine Library Third Party
#################################################################################################################################################

# Engine Third Party Includes
target_include_directories(LayerEngine PUBLIC
    ThirdParty
    ThirdParty/vulkan
    ThirdParty/glm
    ThirdParty/glfw/include
    ThirdParty/glfw/include/GLFW
    ThirdParty/imgui
    ThirdParty/vma
)

# Vulkan (Library)
find_library(Vulkan_LIBRARY NAMES vulkan-1 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vulkan")
target_link_libraries(LayerEngine ${Vulkan_LIBRARY})

# GLFW (Library)
if(MINGW)
    # MinGW needs specfic libglfw3.a file
    find_library(GLFW_LIBRARY NAMES libglfw3 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/lib-mingw-w64")
    target_link_libraries(LayerEngine ${GLFW_LIBRARY})
else()
    find_library(GLFW_LIBRARY NAMES glfw3 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/lib-vc2022")
    target_link_libraries(LayerEngine ${GLFW_LIBRARY})
endif()

# GLM (Source)
file(GLOB_RECURSE GLM_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm/*.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm/*.inl
)
target_sources(LayerEngine PUBLIC ${GLM_SRC})

# ImGui (Source)
file(GLOB ImGui_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/*.h
)
target_sources(LayerEngine PUBLIC ${ImGui_SRC})

# VMA (Source)
target_sources(LayerEngine PUBLIC ThirdParty/vma/vk_mem_alloc.h)