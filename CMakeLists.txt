cmake_minimum_required(VERSION 3.10)
project(Zeroscript)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
add_executable(zero src/main.cpp)
target_link_libraries(zero antlr4-runtime)
add_executable(zrun src/interpreter/main.c)
add_executable(tester src/tester/tester.cpp)

target_link_libraries(zero pthread)
target_link_libraries(zrun pthread)

set_target_properties(zrun PROPERTIES COMPILE_FLAGS "-Ofast ")
set_target_properties(zero PROPERTIES COMPILE_FLAGS "-Ofast ")
