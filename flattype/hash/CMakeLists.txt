# Copyright 2018 Yeolar

#file(GLOB FLATTYPE_HASH_SRCS *.cpp)
file(GLOB FLATTYPE_HASH_HDRS *.h)

#add_library(flattype_hash OBJECT ${FLATTYPE_HASH_SRCS})

install(FILES ${FLATTYPE_HASH_HDRS} DESTINATION include/flattype/hash)
