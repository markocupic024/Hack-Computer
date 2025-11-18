cmake_minimum_required(VERSION 3.10)
project(JackCompiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(JackCompiler
    jack_compiler.cpp
    jack_tokenizer.cpp
    symbol_table.cpp
    vm_writer.cpp
    compilation_engine.cpp
)

