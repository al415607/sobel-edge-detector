cmake_minimum_required(VERSION 3.22.1)

project("sobeledgedetector")

set(PROJECT_ROOT
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../..
)

add_library(${CMAKE_PROJECT_NAME} SHARED
    native-lib.cpp
    ${PROJECT_ROOT}/src/SobelCore.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${PROJECT_ROOT}/include
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    android
    log
    jnigraphics
)