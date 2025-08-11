cmake_minimum_required(VERSION 3.13)

add_subdirectory("thirdParty/glfw")
add_subdirectory("thirdParty/assimp")

project(opeGL)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library(opeGL "${MY_SOURCES}" "${MY_HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c")
target_include_directories(opeGL PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(opeGL PRIVATE glfw assimp)

#add_custom_target(copy_resources ALL
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${PROJECT_SOURCE_DIR}/resources
#        ${PROJECT_BINARY_DIR}/resources
#        COMMENT "Copying resources into binary directory")

#add_dependencies(opeGL copy_resources)

target_compile_options(opeGL PRIVATE -Wall -Wextra -g)
