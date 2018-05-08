# Copyright 2018 Yeolar

file(GLOB FLATTYPE_MATRIX_SRCS *.cpp)
file(GLOB FLATTYPE_MATRIX_HDRS *.h)

add_library(flattype_matrix OBJECT ${FLATTYPE_MATRIX_SRCS})
add_dependencies(flattype_matrix accelerator flatbuffers)

install(FILES ${FLATTYPE_MATRIX_HDRS} DESTINATION include/flattype)
