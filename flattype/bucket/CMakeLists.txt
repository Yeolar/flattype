# Copyright 2018 Yeolar

file(GLOB FLATTYPE_BUCKET_SRCS *.cpp)
file(GLOB FLATTYPE_BUCKET_HDRS *.h)

add_library(flattype_bucket OBJECT ${FLATTYPE_BUCKET_SRCS})
add_dependencies(flattype_bucket accelerator flatbuffers)

install(FILES ${FLATTYPE_BUCKET_HDRS} DESTINATION include/flattype/bucket)
