add_library(bitarray bitarray.c)

set_property(TARGET bitarray PROPERTY POSITION_INDEPENDENT_CODE True)

install(TARGETS bitarray DESTINATION lib)
