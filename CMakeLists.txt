cmake_minimum_required(VERSION 3.2)

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

project(libkmap VERSION 1.0.0 LANGUAGES CXX)

option(LIBKMAP_TESTS "Generate test target" OFF)

add_library(libkmap INTERFACE include/libkmap/libkmap.hpp)
target_include_directories(libkmap INTERFACE include)

if(LIBKMAP_TESTS)
	include(CTest)
	include(GoogleTest)
	enable_testing()

	find_package(GTest REQUIRED)

	add_executable(libkmap_test test/libkmap_test.cpp)
	target_link_libraries(libkmap_test GTest::GTest GTest::Main libkmap)

	gtest_discover_tests(libkmap_test)
endif()
