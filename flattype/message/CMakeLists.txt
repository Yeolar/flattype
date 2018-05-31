# Copyright 2018 Yeolar

file(GLOB FLATTYPE_MESSAGE_SRCS *.cpp)
file(GLOB FLATTYPE_MESSAGE_HDRS *.h)

add_library(flattype_message OBJECT ${FLATTYPE_MESSAGE_SRCS})

install(FILES ${FLATTYPE_MESSAGE_HDRS} DESTINATION include/flattype/message)
