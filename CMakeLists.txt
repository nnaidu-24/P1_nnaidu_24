cmake_minimum_required(VERSION 3.10) 
project(P1)
set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
add_executable(driver driver.cpp)
target_include_directories(driver PUBLIC
                                   "${CMAKE_SOURCE_DIR}/lib/headers"
                                   )

add_subdirectory("lib/source")
target_link_libraries(driver PUBLIC orderbook)
