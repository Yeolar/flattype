# Copyright 2018 Yeolar

file(GLOB FLATTYPE_INDEX_SRCS *.cpp)
file(GLOB FLATTYPE_INDEX_HDRS *.h)

add_library(flattype_index OBJECT ${FLATTYPE_INDEX_SRCS})
add_dependencies(flattype_index accelerator flatbuffers)

install(FILES ${FLATTYPE_INDEX_HDRS} DESTINATION include/flattype/index)
