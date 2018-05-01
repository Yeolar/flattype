# Copyright (C) 2018, Yeolar

# Setup compiler
#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CXX_COMPILER g++)

# Define project
project(flattype)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake-scripts")

# Set project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# Required packages
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
find_package(GFlags REQUIRED)

# Optional packages
find_package(GTest)

# Setup environment
set(CMAKE_BUILD_TYPE Release)   # Debug: -g; Release: -O3 -DNDEBUG
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")    # memcheck
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Add external
include(accelerator)
include(flatbuffers)

# Link libraries
link_libraries(
    ${accelerator_STATIC_LIBRARIES}
    ${flatbuffers_STATIC_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
    ${GFLAGS_LIBRARIES}
)

include_directories(
    ${accelerator_INCLUDE_DIR}
    ${flatbuffers_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}
)

# Check and setup configurations
include(CheckCXXSourceCompiles REQUIRED)

check_cxx_source_compiles("
    #include <string.h>
    #include <errno.h>
    int main() {
        char buf[1024];
        buf[0] = 0;
        int ret = strerror_r(ENOMEM, buf, sizeof(buf));
        return ret;
    }"
    RDD_HAVE_XSI_STRERROR_R)

if(GFLAGS_FOUND)
    set(CMAKE_REQUIRED_LIBRARIES "gflags")

    check_cxx_source_compiles("
        #include <gflags/gflags.h>
        void foo() { google::GetArgv(); }
        int main() { return 0; }"
        RDD_UNUSUAL_GFLAGS_NAMESPACE)

    if(RDD_UNUSUAL_GFLAGS_NAMESPACE)
        set(RDD_GFLAGS_NAMESPACE "google")
    else()
        set(RDD_GFLAGS_NAMESPACE "gflags")
    endif()
endif()

configure_file(
    "${PROJECT_SOURCE_DIR}/flattype/flattype-config.h.in"
    "${PROJECT_SOURCE_DIR}/flattype/flattype-config.h"
)

# Work on all sub directories
#add_subdirectory(flattype)

#add_library(flattype_static STATIC
#    $<TARGET_OBJECTS:flattype>
#)
#set_target_properties(flattype_static PROPERTIES OUTPUT_NAME flattype)
#
#add_library(flattype_shared SHARED
#    $<TARGET_OBJECTS:flattype>
#)
#set_target_properties(flattype_shared PROPERTIES OUTPUT_NAME flattype)

#install(TARGETS flattype_static
#    DESTINATION lib
#    PERMISSIONS OWNER_READ OWNER_WRITE
#                GROUP_READ
#                WORLD_READ
#)
#install(TARGETS flattype_shared
#    DESTINATION lib
#    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
#                GROUP_READ GROUP_EXECUTE
#                WORLD_READ WORLD_EXECUTE
#)

# Binary

# Test
if(GTEST_FOUND)
    enable_testing()
#    add_subdirectory(flattype/test)
endif()

# Pack
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_GENERATOR "DEB")
set(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_PACKAGE_DESCRIPTION "flattype: Flat type based on flatbuffers.")
SET(CPACK_PACKAGE_CONTACT "rdd@rddoc.com")
include(CPack)

