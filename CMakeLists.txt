cmake_minimum_required(VERSION 3.20)

project(SobelEdgeDetector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sobel
    src/main.cpp
)