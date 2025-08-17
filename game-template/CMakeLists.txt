cmake_minimum_required(VERSION 3.13)

add_subdirectory("../opeGL" "../opeGL")

project(game)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_executable(game "${MY_SOURCES}" "${MY_HEADERS}")
target_include_directories(game PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(game PRIVATE opeGL)

add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/../opeGL/resources
        ${PROJECT_BINARY_DIR}/resources
        COMMENT "Copying resources into binary directory")

add_custom_target(copy_resources_local ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources
        ${PROJECT_BINARY_DIR}/resources
        COMMENT "Copying resources into binary directory")

add_dependencies(game copy_resources copy_resources_local)

target_compile_options(game PRIVATE -Wall -Wextra -g)
