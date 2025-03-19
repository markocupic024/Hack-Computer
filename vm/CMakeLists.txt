cmake_minimum_required(VERSION 3.10)

project(VMTranslator)

set(CMAKE_CXX_STANDARD 17)

add_executable(vm
    vm.cpp
    parser.cpp 
)

target_include_directories(vm PRIVATE ${CMAKE_SOURCE_DIR})