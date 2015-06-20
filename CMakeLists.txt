cmake_minimum_required(VERSION 2.8)

project(BIGINT)

include_directories(${BIGINT_SOURCE_DIR})

add_executable(big_integer_testing
               big_integer_testing.cpp
               big_integer.h
               big_integer.cpp
               gtest/gtest-all.cc
               gtest/gtest.h
               gtest/gtest_main.cc opt_vector.h opt_vector.cpp)

add_executable(simpletest
               big_integer.h
               big_integer.cpp simpletest.cpp opt_vector.h opt_vector.cpp)

add_executable(vector_storage
                simpletest.cpp
                big_integer.h
                big_integer.cpp opt_vector.h opt_vector.cpp vector_storage.h vector_storage.cpp)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++11 -pedantic -O3")
endif()

#target_link_libraries(big_integer_testing -lgmp -lgmpxx -lpthread)
