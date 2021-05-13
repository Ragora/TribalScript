cmake_minimum_required (VERSION 3.20.1)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)

# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

project (Torque)
find_package(ANTLR REQUIRED)
find_package(Doxygen
             REQUIRED dot)
             
add_subdirectory(compiler)
