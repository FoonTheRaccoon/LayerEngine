#################################################################################################################################################
# Layer Engine Library
#################################################################################################################################################
add_library(LayerEngine INTERFACE)
set_target_properties(LayerEngine PROPERTIES LINKER_LANGUAGE CXX)

# Engine Includes
target_include_directories(LayerEngine INTERFACE
    #ThirdParty
)

# Engine Headers
target_sources(LayerEngine PUBLIC 
    #libEntry.h
)

# Engine Source
target_sources(LayerEngine PRIVATE 
    #libEntry.cpp
)


#################################################################################################################################################
# Layer Engine Library Third Party
#################################################################################################################################################

# Engine Third Party Includes
target_include_directories(LayerEngine INTERFACE
    ThirdParty
    ThirdParty/vulkan
    ThirdParty/glm
    ThirdParty/GLFW
    ThirdParty/imgui
)

# Vulkan (Library)
find_library(Vulkan_LIBRARY NAMES vulkan-1 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vulkan")
target_link_libraries(LayerEngine INTERFACE ${Vulkan_LIBRARY})

# GLFW (Library)
find_library(GLFW_LIBRARY NAMES glfw3 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/lib")
target_link_libraries(LayerEngine INTERFACE ${GLFW_LIBRARY})

# GLM (CMake)
add_subdirectory(ThirdParty/glm)
target_link_libraries(LayerEngine INTERFACE glm)

# ImGui (Source)
file(GLOB ImGui_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/*.h)
add_library(ImGui STATIC ${ImGui_SRC})
target_include_directories(ImGui PUBLIC ThirdParty)
target_link_libraries(LayerEngine INTERFACE ImGui)