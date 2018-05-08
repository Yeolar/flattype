# Copyright 2018 Yeolar

file(GLOB FLATTYPE_BASE_SRCS *.cpp)
file(GLOB FLATTYPE_BASE_HDRS *.h)

add_library(flattype_base OBJECT ${FLATTYPE_BASE_SRCS})
add_dependencies(flattype_base accelerator flatbuffers)

install(FILES ${FLATTYPE_BASE_HDRS} DESTINATION include/flattype)
