cmake_minimum_required(VERSION 3.5)

project(alliance_it_files LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(alliance_it_files helpers.h helpers.cpp main.cpp)
