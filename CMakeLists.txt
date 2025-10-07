cmake_minimum_required(VERSION 3.20)
project(ccbf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for clangd/Eglot
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optional: clang-tidy if available
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

# Optional: ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Library and executable
add_library(ccbf_lib
  src/ccbf.cpp
  include/ccbf.hpp
)
target_include_directories(ccbf_lib PUBLIC include)

add_executable(ccbf src/main.cpp)
target_link_libraries(ccbf PRIVATE ccbf_lib)

# GoogleTest via FetchContent
include(FetchContent)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(ccbf_tests test/tests.cpp)
target_link_libraries(ccbf_tests PRIVATE ccbf_lib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(ccbf_tests)
