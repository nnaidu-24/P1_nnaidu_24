cmake_minimum_required(VERSION 3.10) 
project(P1)
set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
add_executable(driver driver.cpp)
target_include_directories(driver PUBLIC
                                   "/srv/weka/home_users/nnaidu/P1v2/lib/headers"
                                   )

add_subdirectory("lib/source")
target_link_libraries(driver PUBLIC orderbook)
