cmake_minimum_required(VERSION 3.5)
project(Zeroscript)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

set(ANTLR_TOOL_PATH /usr/local/lib/antlr-4.7.1-complete.jar)

add_custom_target(Antlr
        COMMAND java -jar ${ANTLR_TOOL_PATH} zeroscript.g4 -Dlanguage=Cpp -o src/compiler/parser/generated
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        SOURCES zeroscript.g4)

add_executable(zero src/main.cpp)
target_link_libraries(zero antlr4-runtime)
add_executable(tester src/tester/tester.cpp)

target_link_libraries(zero pthread)

add_dependencies(zero Antlr)

set_target_properties(zero PROPERTIES COMPILE_FLAGS " -Ofast")
