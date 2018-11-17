cmake_minimum_required(VERSION 3.5)

# cmake -Dtest=ON to turn on test mode
option(test "Build all tests." OFF) 

project(libasync)

set(CMAKE_CXX_STANDARD 17)

if(UNIX)
	set(CMAKE_CXX_FLAGS "-std=c++17 -fPIC")
	set(CMAKE_C_FKAGS "-fPIC")
	SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
	SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
endif()


if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4")
endif()

include_directories(./src/interface)
include_directories(./src)
link_directories(./src)
add_subdirectory(./src)

if(test STREQUAL "ON")
    add_subdirectory(googletest) 
    enable_testing()
    add_subdirectory(tests)
else()
    add_subdirectory(samples)
endif()
