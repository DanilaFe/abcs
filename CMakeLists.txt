cmake_minimum_required(VERSION 3.1)
project(abcs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MPFR REQUIRED mpfr)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(external/libabacus)
add_executable(abcs src/main.cpp src/ref.cpp src/abacus.cpp src/types.cpp src/string_functions.cpp src/util.cpp src/operator_functions.cpp src/trig_functions.cpp src/other_functions.cpp)
target_link_libraries(abcs abacus ${MPFR_LIBRARIES} readline)
target_include_directories(abcs PUBLIC include ${MPFR_INCLUDE_DIRS})
target_compile_options(abcs PUBLIC ${MPFR_CFLAGS_OTHER})
