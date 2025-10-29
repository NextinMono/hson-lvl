include(FetchContent)

FetchContent_Declare(glm	GIT_REPOSITORY https://github.com/g-truc/glm.git)
FetchContent_MakeAvailable(glm)

find_package(Vulkan REQUIRED)

add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/imgui")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/imguizmo")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/hedgelib")
set(SDL_VULKAN_ENABLED ON CACHE BOOL "")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/plume")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/SDL")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/imgui-impl-plume")

target_link_libraries(${HE_LEVEL_NAME} PRIVATE imgui imguizmo glm plume Vulkan::Vulkan SDL3::SDL3 HedgeLib imgui-impl-plume)
