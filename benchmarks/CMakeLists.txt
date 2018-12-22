cmake_minimum_required(VERSION 3.5)
project(libasync_benchmarks)
set(CMAKE_CXX_STANDARD 17)

if(UNIX)
	set(CMAKE_CXX_FLAGS "-std=c++17 -fPIC -Wno-reorder")
	SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -Wno-reorder")
	SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall")
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
	set(CMAKE_CXX_FLAGS "/W4 /EHsc")
endif()

include_directories(../libasync/src)
include_directories(/usr/local/tbb/include/tbb)

link_directories(../libasync/build/src/)
link_directories(/usr/local/tbb/lib) 

aux_source_directory(. BENCHMARKSDIRSRCS)
add_executable(benchmarks ${BENCHMARKSDIRSRCS})
find_package(TBB REQUIRED tbb)
target_link_libraries(benchmarks async TBB::tbb)
