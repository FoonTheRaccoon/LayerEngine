#################################################################################################################################################
# Layer Engine Library
#################################################################################################################################################
add_library(LayerEngine INTERFACE)
set_target_properties(LayerEngine PROPERTIES LINKER_LANGUAGE CXX)

# Engine Headers
file(GLOB_RECURSE ENGINE_HEADER *.h)
target_sources(LayerEngine PUBLIC ${ENGINE_HEADER})

# Engine Source
file(GLOB_RECURSE ENGINE_SOURCE *.cpp)
target_sources(LayerEngine PUBLIC ${ENGINE_SOURCE})

# Engine Includes
target_include_directories(LayerEngine INTERFACE
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
target_include_directories(LayerEngine INTERFACE
    ThirdParty
    ThirdParty/vulkan
    ThirdParty/glm
    ThirdParty/glfw/include/GLFW
    ThirdParty/imgui
    ThirdParty/vma
)

# Vulkan (Library)
find_library(Vulkan_LIBRARY NAMES vulkan-1 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vulkan")
target_link_libraries(LayerEngine INTERFACE ${Vulkan_LIBRARY})

# GLFW (Library)
if(MSVC)
    # Non MSVC compilers have an issue linking with gdi32 TODO: Figure out how to link gdi32 with cmake
    find_library(GLFW_LIBRARY NAMES glfw3 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/lib")
    target_link_libraries(LayerEngine INTERFACE ${GLFW_LIBRARY})
else()
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(ThirdParty/glfw)
    target_link_libraries(LayerEngine INTERFACE glfw)
endif()


# GLM (CMake)
add_subdirectory(ThirdParty/glm)
target_link_libraries(LayerEngine INTERFACE glm)

# ImGui (Source)
file(GLOB ImGui_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/*.h)
add_library(ImGui STATIC ${ImGui_SRC})
target_include_directories(ImGui PUBLIC 
    ThirdParty                  # vulkan/vulkan.h
    ThirdParty/glfw/include     # GLFW/glfw3.h
)
target_link_libraries(LayerEngine INTERFACE ImGui)

# VMA (Library)
add_library(vma INTERFACE ThirdParty/vma/vk_mem_alloc.h)
target_link_libraries(LayerEngine INTERFACE vma)