cmake_minimum_required (VERSION 3.0)
project (test_mips32_lexer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(RE2C REQUIRED)

# Run RE2C to generate the lexer
add_custom_command (
    OUTPUT mips32_lexer.cpp
    COMMAND ${RE2C} -W -o mips32_lexer.cpp ${PROJECT_SOURCE_DIR}/../../mips32_lexer.re
    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/../../mips32_lexer.re
)

add_executable(${PROJECT_NAME}  mips32_lexer_test.cpp
                                  mips32_lexer.cpp
                            )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR} )

target_link_libraries(${PROJECT_NAME} PRIVATE doctest)