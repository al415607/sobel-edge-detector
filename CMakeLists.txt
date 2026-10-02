cmake_minimum_required(VERSION 3.20)

project(SobelEdgeDetector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(sobel
    src/main.cpp
    src/SobelFilter.cpp
)

target_include_directories(sobel PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(sobel PRIVATE ${OpenCV_LIBS})