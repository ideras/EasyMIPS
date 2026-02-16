cmake_minimum_required (VERSION 3.0)
project (test_mips32_vm)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(TREECC REQUIRED)
find_package(RE2C REQUIRED)

# Run treecc to generate the AST
add_custom_command (
    OUTPUT mips32_ast.cpp mips32_ast.h
    COMMAND ${TREECC} -o mips32_ast.cpp -h mips32_ast.h ${PROJECT_SOURCE_DIR}/../../mips32_ast.tc
    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/../../mips32_ast.tc
    DEPENDS ${PROJECT_SOURCE_DIR}/../../mips32_ast_compile.tc
)

# Run RE2C to generate the lexer
add_custom_command (
    OUTPUT mips32_lexer.cpp
    COMMAND ${RE2C} -W -o mips32_lexer.cpp ${PROJECT_SOURCE_DIR}/../../mips32_lexer.re
    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/../../mips32_lexer.re
)

add_executable(${PROJECT_NAME}  mips32_vm_test.cpp
                                mips32_lexer.cpp
                                ../../mips32_vm.cpp
                                ../../mips32_runtime.cpp
                                ../../mips32_assembler.cpp
                                ../../mips32_parser.cpp
                                mips32_ast.cpp
                            )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR} )
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../include )

target_link_libraries(${PROJECT_NAME} stdc++fs)