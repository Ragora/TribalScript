project(Parser)

antlr_target(Torque Torque.g4 LISTENER)
include_directories(${ANTLR_Torque_OUTPUT_DIR})

add_library(TorqueCompiler
    compiler.cpp
    ${ANTLR_Torque_CXX_OUTPUTS}
)
target_link_libraries(TorqueCompiler antlr4_static)
target_include_directories(TorqueCompiler PUBLIC
    "include"
)

set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_MAN NO)

doxygen_add_docs(
    TorqueCompilerDocs
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate Documentation for the Compiler"
)
