cmake_minimum_required(VERSION 3.13)

add_subdirectory("../opeGL" "../opeGL")

project(meshConverter)
set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library(meshConverter "${MY_SOURCES}" "${MY_HEADERS}")
target_include_directories(meshConverter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(meshConverter PRIVATE opeGL assimp)
target_compile_options(meshConverter PRIVATE -Wall -Wextra -g)
