cmake_minimum_required(VERSION 3.1)
project(abcs)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(ABCS_SOURCES src/string_functions.cpp src/ref.cpp src/abacus.cpp src/types.cpp src/util.cpp src/operator_functions.cpp src/trig_functions.cpp src/other_functions.cpp)
set(TEST_SOURCES tests/refcount_test.cpp tests/gc_test.cpp tests/trie_test.cpp)

add_subdirectory(external/libabacus)
add_executable(abcs src/main.cpp ${ABCS_SOURCES})
add_executable(abcs-tests tests/test.cpp ${ABCS_SOURCES} ${TEST_SOURCES})

target_link_libraries(abcs abacus mpfr readline)
target_link_libraries(abcs-tests abacus mpfr)
target_include_directories(abcs PUBLIC include)
target_include_directories(abcs-tests PUBLIC include external/greatest)
