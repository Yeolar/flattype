# Find flattype
#
#  FLATTYPE_INCLUDE_DIR - where to find flattype/*.
#  FLATTYPE_LIBRARY     - List of libraries when using flattype.
#  FLATTYPE_FOUND       - True if flattype found.

find_package(PkgConfig)
pkg_check_modules(FLATTYPE QUIET flattype)

if(FLATTYPE_INCLUDE_DIR)
  # Already in cache, be silent
  set(FLATTYPE_FIND_QUIETLY TRUE)
endif()

find_path(FLATTYPE_INCLUDE_DIR flattype/flattype-config.h PATHS
    ${PROJECT_BINARY_DIR}/flattype/flattype/include
    ${PROJECT_BINARY_DIR}-deps/flattype/flattype/include)
find_library(FLATTYPE_LIBRARY flattype PATHS
    ${PROJECT_BINARY_DIR}/flattype/flattype/lib
    ${PROJECT_BINARY_DIR}-deps/flattype/flattype/lib)

# handle the QUIETLY and REQUIRED arguments and set FLATTYPE_FOUND to TRUE 
# if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLATTYPE DEFAULT_MSG FLATTYPE_LIBRARY FLATTYPE_INCLUDE_DIR)

mark_as_advanced(FLATTYPE_LIBRARY FLATTYPE_INCLUDE_DIR)
