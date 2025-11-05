cmake_minimum_required(VERSION 3.13)

project(opeGL)

set(ASSIMP_NO_EXPORT ON CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "")
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "")

add_subdirectory("thirdParty/glfw")
add_subdirectory("thirdParty/assimp")
add_subdirectory("thirdParty/tmxlite")

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE MY_RESOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")

add_library(opeGL "${MY_SOURCES}" "${MY_HEADERS}" "${MY_RESOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c")
target_include_directories(opeGL PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(opeGL PUBLIC glfw assimp tmxlite)
target_compile_options(opeGL PRIVATE -Wall -Wextra -g)
