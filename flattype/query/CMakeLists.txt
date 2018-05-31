# Copyright 2018 Yeolar

file(GLOB FLATTYPE_QUERY_SRCS *.cpp)
file(GLOB FLATTYPE_QUERY_HDRS *.h)

add_library(flattype_query OBJECT ${FLATTYPE_QUERY_SRCS})

install(FILES ${FLATTYPE_QUERY_HDRS} DESTINATION include/flattype/query)
