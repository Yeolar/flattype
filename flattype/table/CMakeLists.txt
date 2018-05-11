# Copyright 2018 Yeolar

file(GLOB FLATTYPE_TABLE_SRCS *.cpp)
file(GLOB FLATTYPE_TABLE_HDRS *.h)

add_library(flattype_table OBJECT ${FLATTYPE_TABLE_SRCS})
add_dependencies(flattype_table accelerator flatbuffers)

install(FILES ${FLATTYPE_TABLE_HDRS} DESTINATION include/flattype/table)
