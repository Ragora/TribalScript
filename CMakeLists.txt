if (POLICY CMP0114)
	cmake_policy(SET CMP0114 OLD)
endif (POLICY CMP0114)

if (POLICY CMP0113)
	cmake_policy(SET CMP0113 OLD)
endif (POLICY CMP0113)

if (POLICY CMP0048)
	cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(TorqueScript VERSION 0.1 LANGUAGES CXX)

cmake_minimum_required (VERSION 3.16.3)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)

# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

find_package(ANTLR REQUIRED)
find_package(Doxygen)

add_subdirectory(source)

enable_testing ()
add_subdirectory(tests)
