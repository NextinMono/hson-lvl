cmake_minimum_required (VERSION 3.20)

set(HE_LEVEL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(HE_LEVEL_EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)
set(HE_LEVEL_NAME hson-lvl)

if(POLICY CMP0079)
  cmake_policy(SET CMP0079 NEW)
endif()

project("${HE_LEVEL_NAME}")

add_subdirectory(${HE_LEVEL_SOURCE_DIR})
add_subdirectory(${HE_LEVEL_EXTERNAL_DIR})
