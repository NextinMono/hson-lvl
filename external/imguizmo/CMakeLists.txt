project("imguizmo")

file(GLOB IMGUI_SOURCES
    ImGuizmo/*.cpp
)

add_library(imguizmo STATIC ${IMGUI_SOURCES})

target_include_directories(imguizmo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo
)

target_link_libraries(imguizmo PUBLIC imgui)
