project("imgui")

file(GLOB IMGUI_SOURCES
    imgui/*.cpp
    imgui/backends/imgui_impl_vulkan.cpp
    imgui/backends/imgui_impl_sdl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

target_link_libraries(imgui PUBLIC SDL3::SDL3 Vulkan::Vulkan)
