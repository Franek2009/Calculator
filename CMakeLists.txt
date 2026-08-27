cmake_minimum_required(VERSION 3.21)

project(Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Calculator
    src/main.cpp
)

target_link_libraries(Calculator PRIVATE Qt6::Widgets)
