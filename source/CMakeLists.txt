antlr_target(Torque Torque.g4 LISTENER)
include_directories(${ANTLR_Torque_OUTPUT_DIR})

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/source/*.cpp)
add_library(TorqueCompiler
    ${SOURCES}
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
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/source/doxymain.h
        ${SOURCES}
        COMMENT "Generate Documentation for the Compiler"
    )
endif (${DOXYGEN_FOUND})
