cmake_minimum_required(VERSION 3.10)
project(AntlrParserGenerator)

find_package(Java REQUIRED)

set(ANTLR_JAR ${PROJECT_SOURCE_DIR}/antlr-tools/antlr-3.4-complete.jar)

set(GRAMMAR_DIR ${PROJECT_SOURCE_DIR}/grammar-spec)

set(GRAMMAR_FILE ${GRAMMAR_DIR}/Lab1Grammar.g) 

set(ATNLR_GEN_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/generated)

set (ANTLR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/libantlr3c-3.4)
set (ANTLR_BUILD_DIR ${ANTLR_SOURCE_DIR}/build)

file(MAKE_DIRECTORY ${ATNLR_GEN_OUTPUT_DIR})


set(GENERATED_SOURCES_ANTRL
    ${ATNLR_GEN_OUTPUT_DIR}/Lab1GrammarLexer.h
    ${ATNLR_GEN_OUTPUT_DIR}/Lab1GrammarParser.h
    ${ATNLR_GEN_OUTPUT_DIR}/Lab1GrammarLexer.c
    ${ATNLR_GEN_OUTPUT_DIR}/Lab1GrammarParser.c
)

add_custom_command(
    OUTPUT ${GENERATED_SOURCES_ANTRL}
    COMMAND ${Java_JAVA_EXECUTABLE} -cp ${ANTLR_JAR} org.antlr.Tool -o ${ATNLR_GEN_OUTPUT_DIR} ${GRAMMAR_FILE}
    DEPENDS ${GRAMMAR_FILE}
    COMMENT "Running ANTLR to generate lexer and parser from grammar"
)

add_custom_target(GenerateParser DEPENDS ${GENERATED_SOURCES_ANTRL})

include_directories(${ANTLR_BUILD_DIR}/include)
link_directories(${ANTLR_BUILD_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/generated)

add_executable(main src/main.c ${GENERATED_SOURCES_ANTRL}
        src/reader.c
        src/reader.h
        src/parser.c
        src/parser.h
        src/ast.c
        src/ast.h
        src/cfg.c
        src/cfg.h
        src/err_list.c
        src/err_list.h
        src/ot.c
        src/ot.h
        src/type.c
        src/type.h
        src/tables.c
        src/tables.h
        src/codegen.c
        src/codegen.h)

target_link_libraries(main antlr3c)
execute_process(COMMAND bash -c "cd \"${ANTLR_SOURCE_DIR}\" && chmod u+x ./build-antlr.sh && ./build-antlr.sh ${ANTLR_BUILD_DIR}")

message(STATUS "CMake project has been successfully configured.")
