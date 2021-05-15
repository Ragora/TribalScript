antlr_target(Torque Torque.g4 LISTENER)
include_directories(${ANTLR_Torque_OUTPUT_DIR})

add_library(TorqueCompiler
    compiler.cpp
    interpreter.cpp
    executionscope.cpp
    codeblock.cpp
    stringhelpers.cpp
    function.cpp
    storedfloatvalue.cpp
    storedstringvalue.cpp
    storedintegervalue.cpp
    storedvalue.cpp
    storedglobalreferencevalue.cpp
    storedlocalreferencevalue.cpp
    ${ANTLR_Torque_CXX_OUTPUTS}
)
target_link_libraries(TorqueCompiler antlr4_static)
target_include_directories(TorqueCompiler PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${ANTLR_Torque_OUTPUT_DIR}
)

add_executable(main main.cpp)
target_link_libraries(main TorqueCompiler)

if (${DOXYGEN_FOUND})
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN NO)

    doxygen_add_docs(
        TorqueCompilerDocs
        ${PROJECT_SOURCE_DIR}
        COMMENT "Generate Documentation for the Compiler"
    )
endif (${DOXYGEN_FOUND})
