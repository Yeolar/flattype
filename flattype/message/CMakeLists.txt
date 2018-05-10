# Copyright 2018 Yeolar

file(GLOB FLATTYPE_MESSAGE_SRCS *.cpp)
file(GLOB FLATTYPE_MESSAGE_HDRS *.h)

add_library(flattype_message OBJECT ${FLATTYPE_MESSAGE_SRCS})
add_dependencies(flattype_message accelerator flatbuffers)

install(FILES ${FLATTYPE_MESSAGE_HDRS} DESTINATION include/flattype/message)
