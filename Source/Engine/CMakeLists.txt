#################################################################################################################################################
# Layer Engine Library
#################################################################################################################################################
add_library(LayerEngine STATIC
        # Engine Source
        _cpp/Engine.cpp
        _cpp/MeshRenderer.cpp
        _cpp/MeshData.cpp
        _cpp/Actor.cpp
        _cpp/Level.cpp
        _cpp/SubLevel.cpp
        _cpp/ComponentHandle.cpp
        _cpp/ECSManager.cpp
        _cpp/EntityHandle.cpp
        _cpp/VkBuffersAndImages.cpp
        _cpp/GraphicsPipeline.cpp
        _cpp/RenderPass.cpp
        _cpp/SwapChain.cpp
        _cpp/VkSetup.cpp
        _cpp/RenderManager.cpp
        _cpp/Viewport.cpp
        _cpp/Timer.cpp
        _cpp/FileHelper.cpp
        _cpp/StringHelper.cpp
        _cpp/Logger.cpp
        _cpp/LoggingCallbacks.cpp
        _cpp/GpuMemoryTracker.cpp
        _cpp/MemoryTracker.cpp
        _cpp/ImGuiManager.cpp
        # Engine Headers
        Engine.h

        Components/ComponentHandle.h
        Components/MeshRenderer.h

        Entities/EntityHandle.h
        Entities/Actor.h
        Entities/Level.h
        Entities/SubLevel.h

        Managers/ECSManager.h
        Managers/ImGuiManager.h

        Render/Vulkan/GraphicsPipeline.h
        Render/Vulkan/RenderPass.h
        Render/Vulkan/SwapChain.h
        Render/Vulkan/VkBuffersAndImages.h
        Render/Vulkan/VkConfig.h
        Render/Vulkan/VkSetup.h
        Render/RenderManager.h
        Render/Viewport.h

        Utilities/Events/Broadcaster.h
        Utilities/Helpers/FileHelper.h
        Utilities/Helpers/StringHelper.h
        Utilities/Helpers/Timer.h
        Utilities/Logger/Logger.h
        Utilities/Logger/LoggingCallbacks.h
        Utilities/Memory/GpuMemoryTracker.h
        Utilities/Memory/LayerContainers.h
        Utilities/Memory/LayerMemory.h
        Utilities/Memory/MemoryTracker.h
        Utilities/Types/HelperTypes.h
        Utilities/Types/VkTypes.h
        Utilities/EngUtils.h

        _ThirdParty/ThirdParty.h
        _ThirdParty/ThirdPartyConfig.h
        _ThirdParty/ConstantsAndAliases.h
)

set_target_properties(LayerEngine PROPERTIES LINKER_LANGUAGE CXX)

# Engine Includes
target_include_directories(LayerEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    Components
    Data
    Entities
    Managers
    Render
    Render/Vulkan
    Utilities
    Utilities/Events
    Utilities/Helpers
    Utilities/Logger
    Utilities/Memory
    Utilities/Types
)


#################################################################################################################################################
# Layer Engine Library Third Party And PCH
#################################################################################################################################################

# Engine PCH for all third party includes
target_precompile_headers(LayerEngine PUBLIC _ThirdParty/ThirdParty.h)

# Engine Third Party Includes
target_include_directories(LayerEngine PUBLIC
    _ThirdParty
    _ThirdParty/vulkan
    _ThirdParty/glm
    _ThirdParty/glfw/include
    _ThirdParty/glfw/include/GLFW
    _ThirdParty/imgui
    _ThirdParty/vma
)

# Vulkan (Library)
find_library(Vulkan_LIBRARY NAMES vulkan-1 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/_ThirdParty/vulkan")
target_link_libraries(LayerEngine ${Vulkan_LIBRARY})

# GLFW (Library)
if(MINGW)
    # MinGW needs specific libglfw3.a file
    find_library(GLFW_LIBRARY NAMES libglfw3 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/_ThirdParty/glfw/lib-mingw-w64")
    target_link_libraries(LayerEngine ${GLFW_LIBRARY})
else()
    find_library(GLFW_LIBRARY NAMES glfw3 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/_ThirdParty/glfw/lib-vc2022")
    target_link_libraries(LayerEngine ${GLFW_LIBRARY})
endif()

# GLM (Source)
add_subdirectory(_ThirdParty/glm)
target_link_libraries(LayerEngine glm)

# ImGui (Source)
file(GLOB ImGui_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/_ThirdParty/imgui/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/_ThirdParty/imgui/*.h
)
target_sources(LayerEngine PUBLIC ${ImGui_SRC})

# VMA (Source)
target_sources(LayerEngine 
    PRIVATE _ThirdParty/vma/vma_imp.cpp     # VMA needs a define in a single .cpp file that doesn't play nice with pch, so we do it here
    PUBLIC _ThirdParty/vma/vk_mem_alloc.h
)