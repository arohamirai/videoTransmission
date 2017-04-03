###############################################################################
# Find cnettran
#
# This sets the following variables:
# CNETTRAN_FOUND - True if CNETTRAN was found.
# CNETTRAN_INCLUDE_DIRS - Directories containing the CNETTRAN include files.
# CNETTRAN_LIBRARIES - Libraries needed to use CNETTRAN.

find_path(CNETTRAN_INCLUDE_DIR CNetTran.h
          PATHS
            ${CMAKE_CURRENT_SOURCE_DIR}/../CNetTran
)

find_library(CNETTRAN_LIBRARY
             NAMES libcnettran.so
             PATHS
               ${CMAKE_CURRENT_SOURCE_DIR}/../CNetTran
               ${CMAKE_CURRENT_SOURCE_DIR}/../CNetTran/build
             PATH_SUFFIXES ${CNETTRAN_PATH_SUFFIXES}
)

set(CNETTRAN_INCLUDE_DIRS ${CNETTRAN_INCLUDE_DIR})
set(CNETTRAN_LIBRARIES ${CNETTRAN_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CNETTRAN DEFAULT_MSG CNETTRAN_LIBRARY CNETTRAN_INCLUDE_DIR)

if(NOT WIN32)
  mark_as_advanced(CNETTRAN_LIBRARY CNETTRAN_INCLUDE_DIR)
endif()
