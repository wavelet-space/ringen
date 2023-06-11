
cmake_minimum_required(VERSION 3.20)

project(
	Ringen
	VERSION "0.1.0"
	DESCRIPTION "Ring buffer, disruptor by LMAX in C++"
	HOMEPAGE_URL "https://github.com/wavelet-space/ringen"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
	Ringen
	INTERFACE
)

target_include_directories(
  Ringen
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(Ringen INTERFACE cxx_std_20)

add_executable(
  example1
  examples/example1.cpp
)

target_include_directories(
  example1
  PRIVATE include
)

# set_property(TARGET ringen_example PROPERTY CXX_STANDARD 11)