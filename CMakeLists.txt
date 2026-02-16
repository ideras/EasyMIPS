cmake_minimum_required (VERSION 3.14)
project (EasyMIPS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include (TestBigEndian)

TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    add_definitions(-D__BYTE_ORDER=__BIG_ENDIAN)
else()
    add_definitions(-D__BYTE_ORDER=__LITTLE_ENDIAN)
endif()

# Try to find replxx first
find_package(replxx QUIET)

# If not found, use FetchContent to download it
if(NOT replxx_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        replxx
        GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git
        GIT_TAG master
    )

    FetchContent_MakeAvailable(replxx)

    set(REPLXX_LIBRARY replxx)
endif()

find_package(RE2C REQUIRED)
find_package(TREECC REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/EasyMIPS)

# Run RE2C to generate the lexer
add_custom_command (
    OUTPUT mips32_lexer.cpp
    COMMAND ${RE2C} -o mips32_lexer.cpp ${PROJECT_SOURCE_DIR}/src/mips32_lexer.re
    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/src/mips32_lexer.re
)

# Run treecc to generate the AST
add_custom_command (
    OUTPUT mips32_ast.cpp mips32_ast.h
    COMMAND ${TREECC} -o mips32_ast.cpp -h mips32_ast.h ${PROJECT_SOURCE_DIR}/include/EasyMIPS/mips32_ast.tc
    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/include/EasyMIPS/mips32_ast.tc
    DEPENDS ${PROJECT_SOURCE_DIR}/include/EasyMIPS/mips32_ast_compile.tc
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_definitions(-D_WIN32_WINNT_VISTA=0x0600)
    add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_VISTA)
endif()

add_executable(EasyMIPS mips32_ast.cpp
                        mips32_ast.h
                        mips32_lexer.cpp
                        src/mips32_parser.cpp
                        src/mips32_assembler.cpp
                        src/mips32_runtime.cpp
                        src/mips32_vm.cpp
                        src/mips32_completion.cpp
                        src/easm_clargs.cpp
                        src/easm_error.cpp
                        src/native_lib.cpp
                        src/main.cpp)

target_link_libraries(${PROJECT_NAME} replxx)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} -ldl)
endif ()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        target_compile_options(${PROJECT_NAME} PRIVATE -static)
        target_link_options(${PROJECT_NAME} PRIVATE -static)
    endif()
    target_link_libraries(${PROJECT_NAME} -static-libgcc)
endif()

add_subdirectory(tests)
