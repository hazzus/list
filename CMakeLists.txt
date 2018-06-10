cmake_minimum_required(VERSION 2.8)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -O0 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

project(my_list_proj)

add_library(my_list list.h)
set_target_properties(my_list PROPERTIES LINKER_LANGUAGE CXX)
add_executable(${PROJECT_NAME}  test.cpp gtest/gtest_main.cc
    gtest/gtest-all.cc gtest/gtest.h )
target_link_libraries(${PROJECT_NAME} -lgmp -lgmpxx -lpthread)
target_link_libraries(${PROJECT_NAME} my_list)

