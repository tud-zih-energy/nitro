include("${CMAKE_CURRENT_LIST_DIR}/NitroTargets.cmake")

get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

message(STATUS "Using Nitro: ${_IMPORT_PREFIX}")

set(_IMPORT_PREFIX)
