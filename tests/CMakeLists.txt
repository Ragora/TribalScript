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
target_link_libraries(IfTest TorqueCompiler gtest_main)
add_test(NAME IfTest COMMAND IfTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(WhileTest while.cpp)
target_link_libraries(WhileTest TorqueCompiler gtest_main)
add_test(NAME WhileTest COMMAND WhileTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(ForTest for.cpp)
target_link_libraries(ForTest TorqueCompiler gtest_main)
add_test(NAME ForTest COMMAND ForTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(PackageTest package.cpp)
target_link_libraries(PackageTest TorqueCompiler gtest_main)
add_test(NAME PackageTest COMMAND PackageTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(VariablesTest variables.cpp)
target_link_libraries(VariablesTest TorqueCompiler gtest_main)
add_test(NAME VariablesTest COMMAND VariablesTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(CombinedTest combined.cpp)
target_link_libraries(CombinedTest TorqueCompiler gtest_main)
add_test(NAME CombinedTest COMMAND CombinedTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(ArrayTest array.cpp)
target_link_libraries(ArrayTest TorqueCompiler gtest_main)
add_test(NAME ArrayTest COMMAND ArrayTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(SwitchTest switch.cpp)
target_link_libraries(SwitchTest TorqueCompiler gtest_main)
add_test(NAME SwitchTest COMMAND SwitchTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(OpOrderTest opOrder.cpp)
target_link_libraries(OpOrderTest TorqueCompiler gtest_main)
add_test(NAME OpOrderTest COMMAND OpOrderTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(CaseSensitivityTest caseSensitive.cpp)
target_link_libraries(CaseSensitivityTest TorqueCompiler gtest_main)
add_test(NAME CaseSensitivityTest COMMAND CaseSensitivityTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(MemoryReferenceTest memoryReference.cpp)
target_link_libraries(MemoryReferenceTest TorqueCompiler gtest_main)
add_test(NAME MemoryReferenceTest COMMAND MemoryReferenceTest WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
