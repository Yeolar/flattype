# Copyright 2018 Yeolar

file(GLOB FLATTYPE_OBJECT_SRCS *.cpp)
file(GLOB FLATTYPE_OBJECT_HDRS *.h)

add_library(flattype_object OBJECT ${FLATTYPE_OBJECT_SRCS})

install(FILES ${FLATTYPE_OBJECT_HDRS} DESTINATION include/flattype/object)
