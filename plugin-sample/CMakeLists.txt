cmake_minimum_required (VERSION 3.0)
project (sc-plugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/../pdk)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
add_definitions(-D_WIN32_WINNT_VISTA=0x0600)
add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_VISTA)
endif()

add_library(${PROJECT_NAME} SHARED plugin_main.cpp)

target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)