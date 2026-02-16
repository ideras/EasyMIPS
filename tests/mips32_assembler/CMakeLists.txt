cmake_minimum_required (VERSION 3.0)
project (test_mips32_assembler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program (TREECC treecc
              PATHS /usr /usr/local /opt /opt/local $ENV{TREECC_DIR}
              PATH_SUFFIXES treecc/bin bin )

if (TREECC)
    message(STATUS "Using treecc executable ${TREECC}")
else(TREECC)
    message(FATAL_ERROR "treecc was not found. Try setting TREECC_DIR environment variable")
endif(TREECC)

# Run treecc to generate the AST
add_custom_command (
    OUTPUT mips32_ast.cpp mips32_ast.h
    COMMAND ${TREECC} -o mips32_ast.cpp -h mips32_ast.h ${PROJECT_SOURCE_DIR}/../../mips32_ast.tc
    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/../../mips32_ast.tc
    DEPENDS ${PROJECT_SOURCE_DIR}/../../mips32_ast_compile.tc
)

add_executable(${PROJECT_NAME}  mips32_assembler_test.cpp
                                mips32_ast.cpp
                                ../../mips32_runtime.cpp
                                ../../mips32_assembler.cpp
                            )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR} )
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../include )