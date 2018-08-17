cmake_minimum_required(VERSION 3.1)
project(cmdAbacus)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MPFR REQUIRED mpfr)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(external/libabacus)
add_executable(cmdAbacus src/main.cpp)
target_link_libraries(cmdAbacus abacus ${MPFR_LIBRARIES} readline)
target_include_directories(cmdAbacus PUBLIC include ${MPFR_INCLUDE_DIRS})
target_compile_options(cmdAbacus PUBLIC ${MPFR_CFLAGS_OTHER})
