cmake_minimum_required(VERSION 3.18)
project(symresolvexx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CONFIGURATION_TYPES Release Debug)

add_library(symresolvexx STATIC)

set_target_properties(symresolvexx PROPERTIES PUBLIC_HEADER 
    include/symresolvexx/symresolvexx.h
)

target_include_directories(symresolvexx PUBLIC include)

add_subdirectory(src)
