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

include(FetchContent)
FetchContent_Declare(
    googletest
    # Specify the commit you depend on and update it regularly.
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Init all tests
add_executable(IfTest if.cpp)
target_link_libraries(IfTest TribalScript gtest_main)
add_test(NAME IfTest COMMAND IfTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(WhileTest while.cpp)
target_link_libraries(WhileTest TribalScript gtest_main)
add_test(NAME WhileTest COMMAND WhileTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(ForTest for.cpp)
target_link_libraries(ForTest TribalScript gtest_main)
add_test(NAME ForTest COMMAND ForTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(PackageTest package.cpp)
target_link_libraries(PackageTest TribalScript gtest_main)
add_test(NAME PackageTest COMMAND PackageTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(VariablesTest variables.cpp)
target_link_libraries(VariablesTest TribalScript gtest_main)
add_test(NAME VariablesTest COMMAND VariablesTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(CombinedTest combined.cpp)
target_link_libraries(CombinedTest TribalScript gtest_main)
add_test(NAME CombinedTest COMMAND CombinedTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(ArrayTest array.cpp)
target_link_libraries(ArrayTest TribalScript gtest_main)
add_test(NAME ArrayTest COMMAND ArrayTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(SwitchTest switch.cpp)
target_link_libraries(SwitchTest TribalScript gtest_main)
add_test(NAME SwitchTest COMMAND SwitchTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(OpOrderTest opOrder.cpp)
target_link_libraries(OpOrderTest TribalScript gtest_main)
add_test(NAME OpOrderTest COMMAND OpOrderTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(CaseSensitivityTest caseSensitive.cpp)
target_link_libraries(CaseSensitivityTest TribalScript gtest_main)
add_test(NAME CaseSensitivityTest COMMAND CaseSensitivityTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(MemoryReferenceTest memoryReference.cpp)
target_link_libraries(MemoryReferenceTest TribalScript gtest_main)
add_test(NAME MemoryReferenceTest COMMAND MemoryReferenceTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
