#!/bin/bash

if [ -z $1 ]; then
    echo "Please specify the build directory"
    exit 1
fi

if [ -z $2 ]; then
    echo "Please specify the tests directory"
    exit 1
fi

BUILD_DIR=$1
TEST_DIR=$2

$BUILD_DIR/tests/test-mem_iterator 
$BUILD_DIR/tests/test-mips32_lexer 
$BUILD_DIR/tests/test-mips32_parser --include=$TEST_DIR/mips32_parser/files
$BUILD_DIR/tests/test-mips32_vm --include=$TEST_DIR/mips32_vm/files
$BUILD_DIR/tests/test-mips32_assembler 