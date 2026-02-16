cmake_minimum_required (VERSION 3.14)

project(testcases)

set(TEST_CASES test-mem_iterator
               test-mips32_lexer
               test-mips32_parser
               test-mips32_assembler
               test-mips32_vm)

if (${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Cannot build the test-cases separated")
endif()

# Download doctest header file directly
set(DOCTEST_VERSION "2.4.12")
set(DOCTEST_URL "https://github.com/doctest/doctest/releases/download/v${DOCTEST_VERSION}/doctest.h")
set(DOCTEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/doctest")
set(DOCTEST_INCLUDE_DIR "${DOCTEST_DIR}")

message(STATUS "DOCTEST_INCLUDE_DIR: ${DOCTEST_INCLUDE_DIR}")

# Download doctest.h if it doesn't exist
if(NOT EXISTS "${DOCTEST_INCLUDE_DIR}/doctest.h")
    message(STATUS "Downloading doctest.h v${DOCTEST_VERSION}...")
    file(DOWNLOAD ${DOCTEST_URL} ${DOCTEST_INCLUDE_DIR}/doctest.h
         STATUS status
         TLS_VERIFY ON)
    list(GET status 0 error_code)
    list(GET status 1 error_msg)
    if(NOT error_code EQUAL 0)
        message(FATAL_ERROR "Failed to download doctest.h: ${error_msg}")
    endif()
    message(STATUS "doctest.h downloaded successfully")
endif()

# Create an interface library for doctest
add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE ${DOCTEST_INCLUDE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/EasyMIPS)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(mips32_lexer OBJECT mips32_lexer.cpp)

add_library(mips32_parser OBJECT ${CMAKE_SOURCE_DIR}/src/mips32_parser.cpp mips32_ast.h)

add_library(easm_error OBJECT ${CMAKE_SOURCE_DIR}/src/easm_error.cpp)

add_library(mips32_ast OBJECT mips32_ast.cpp mips32_ast.h)

add_library(mips32_asm OBJECT   ${CMAKE_SOURCE_DIR}/src/mips32_runtime.cpp
                                ${CMAKE_SOURCE_DIR}/src/mips32_assembler.cpp)

# Memory Iterator test
# ====================

add_executable(test-mem_iterator mem_iterator/test-mem_iterator.cpp)

target_link_libraries(test-mem_iterator PRIVATE doctest)

# MIPS32 Lexer test
# =================

add_custom_command (
    OUTPUT mips32_lexer.cpp
    COMMAND ${RE2C} -W -o mips32_lexer.cpp ${CMAKE_SOURCE_DIR}/src/mips32_lexer.re
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/src/mips32_lexer.re
)

add_executable(test-mips32_lexer    mips32_lexer/mips32_lexer_test.cpp
                                    $<TARGET_OBJECTS:mips32_lexer>)

target_link_libraries(test-mips32_lexer PRIVATE doctest)

# MIPS32 Parser test
# ==================

add_custom_command (
    OUTPUT mips32_ast.cpp mips32_ast.h
    COMMAND ${TREECC} -o mips32_ast.cpp -h mips32_ast.h ${CMAKE_SOURCE_DIR}/include/EasyMIPS/mips32_ast.tc
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/include/EasyMIPS/mips32_ast.tc
    DEPENDS ${CMAKE_SOURCE_DIR}/include/EasyMIPS/mips32_ast_compile.tc
)

add_executable(test-mips32_parser   mips32_parser/mips32_parser_test.cpp
                                    $<TARGET_OBJECTS:easm_error>
                                    $<TARGET_OBJECTS:mips32_lexer>
                                    $<TARGET_OBJECTS:mips32_parser>
                                    $<TARGET_OBJECTS:mips32_ast>
                                    $<TARGET_OBJECTS:mips32_asm>)

target_link_libraries(test-mips32_parser PRIVATE doctest)

# MIPS32 Assembler test
# =====================
add_executable(test-mips32_assembler    mips32_assembler/mips32_assembler_test.cpp
                                        $<TARGET_OBJECTS:easm_error>
                                        $<TARGET_OBJECTS:mips32_ast>
                                        $<TARGET_OBJECTS:mips32_asm>)

target_link_libraries(test-mips32_assembler PRIVATE doctest)

# MIPS32 Virtual Machine test
# ===========================

add_executable(test-mips32_vm   mips32_vm/mips32_vm_test.cpp
                                $<TARGET_OBJECTS:easm_error>
                                $<TARGET_OBJECTS:mips32_lexer>
                                $<TARGET_OBJECTS:mips32_parser>
                                $<TARGET_OBJECTS:mips32_ast>
                                $<TARGET_OBJECTS:mips32_asm>
                                ${CMAKE_SOURCE_DIR}/src/mips32_vm.cpp)

target_link_libraries(test-mips32_vm PRIVATE doctest)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    foreach(TC IN LISTS TEST_CASES)
        if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
            target_compile_options(${TC} PRIVATE -static)
            target_link_options(${TC} PRIVATE -static)
        endif()

        target_link_libraries(${TC} PRIVATE -static-libgcc)
    endforeach()
endif()