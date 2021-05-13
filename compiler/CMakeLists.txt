project(Parser)

antlr_target(Torque Torque.g4 LISTENER)
include_directories(${ANTLR_Torque_OUTPUT_DIR})

add_library(TorqueCompiler
    compiler.cpp
    compiler.h
    ${ANTLR_Torque_CXX_OUTPUTS}
)
target_link_libraries(TorqueCompiler antlr4_static)
