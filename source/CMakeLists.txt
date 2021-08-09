# Copyright 2021 Robert MacGregor
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

antlr_target(Tribes2 Tribes2.g4 VISITOR)
include_directories(${ANTLR_Tribes2_OUTPUT_DIR})

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/source/*.cpp ${PROJECT_SOURCE_DIR}/source/libraries/*.cpp)
file(GLOB INCLUDES ${PROJECT_SOURCE_DIR}/include/tribalscript/*.hpp ${PROJECT_SOURCE_DIR}/include/tribalscript/libraries/*.hpp)
add_library(TribalScript
    ${SOURCES}
    ${INCLUDES}
    ${ANTLR_Tribes2_CXX_OUTPUTS}
)
target_link_libraries(TribalScript antlr4_static)
target_include_directories(TribalScript PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${ANTLR_Tribes2_OUTPUT_DIR}
)

add_executable(main main.cpp)
target_link_libraries(main TribalScript)
