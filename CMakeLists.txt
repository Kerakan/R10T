cmake_minimum_required(VERSION 3.15)
project(Autobattler VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_subdirectory(include/raylib)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX "build/")
list(FILTER SOURCES EXCLUDE REGEX "include/raylib/")

add_executable(autobattler ${SOURCES})
add_custom_command(TARGET autobattler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:autobattler>/data"
    COMMENT "Copying data directory to output folder"
)
target_include_directories(autobattler PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/raylib/src
)
target_link_libraries(autobattler raylib)
if(MSVC)
    target_compile_options(autobattler PRIVATE /W4 /permissive-)
else()
    target_compile_options(autobattler PRIVATE -Wall -Wextra -Wpedantic)
endif()