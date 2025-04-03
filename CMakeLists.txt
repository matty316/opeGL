cmake_minimum_required(VERSION 3.13)

add_subdirectory("thirdParty/glfw")
add_subdirectory("thirdParty/glm")

project(opeGL)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(opeGL "${MY_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c")
target_include_directories(opeGL PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(opeGL PRIVATE glfw glm)

