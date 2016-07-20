cmake_minimum_required(VERSION 3.3)
project(cbitarray C)

option(BUILD_SHARED_LIBS "Build as a shared library." OFF)

if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -mpopcnt")
  set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} -O3 -mtune=native -march=native")
endif()

set(CMAKE_C_STANDARD 11)

include_directories(include)
install(FILES include/bitarray.h DESTINATION include)

add_subdirectory(src)

add_subdirectory(bench)
add_subdirectory(test)
