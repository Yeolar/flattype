# Copyright 2018 Yeolar

file(GLOB FLATTYPE_IDLS *.fbs)

foreach(fbs ${FLATTYPE_IDLS})
    execute_process(COMMAND
        ${FLATBUFFERS_FLATC}
            --cpp
            --scoped-enums
            --gen-mutable
            --gen-name-strings
            --gen-object-api
            --include-prefix flattype/idl/
            #--cpp-str-type acc::fbstring
            -o ${PROJECT_SOURCE_DIR}/flattype/idl/
            -I ${PROJECT_SOURCE_DIR}/flattype/idl/
            ${fbs})
endforeach()

file(GLOB FLATTYPE_IDL_HDRS *.h)

install(FILES ${FLATTYPE_IDL_HDRS} DESTINATION include/flattype/idl)
