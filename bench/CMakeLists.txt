find_library(
  LIBCBENCH_LIBS
  NAMES cbench libcbench
  )

find_path(
  CBENCH_INCLUDE_DIR
  cbench.h
  )

add_executable(bench-cbitarray bench-bitarray.c)

target_link_libraries(bench-cbitarray ${LIBCBENCH_LIBS})
target_include_directories(bench-cbitarray PUBLIC ${CBENCH_INCLUDE_DIR})

target_link_libraries(bench-cbitarray bitarray)
