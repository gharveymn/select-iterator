add_library (select-iterator INTERFACE)

target_sources (
  select-iterator
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/gch/select-iterator.hpp>
)

target_include_directories (
  select-iterator
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_include_directories (
  select-iterator
  SYSTEM INTERFACE
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)

set_target_properties (
  select-iterator
  PROPERTIES
  PUBLIC_HEADER
    include/gch/select-iterator.hpp
)

add_library (gch::select-iterator ALIAS select-iterator)

install (
  TARGETS
    select-iterator
  EXPORT
    select-iterator-targets
  PUBLIC_HEADER
    DESTINATION include/gch
)

file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/select-iterator-config.cmake [=[
get_filename_component (PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)
include ("${CMAKE_CURRENT_LIST_DIR}/select-iterator-targets.cmake")
]=])

include (CMakePackageConfigHelpers)
write_basic_package_version_file (
  select-iterator-version.cmake
  COMPATIBILITY
    SameMinorVersion
)

install (
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/select-iterator-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/select-iterator-version.cmake
  DESTINATION
    lib/cmake/select-iterator
)

install (
  EXPORT
    select-iterator-targets
  DESTINATION
    lib/cmake/select-iterator
  NAMESPACE
    gch::
)

if (GCH_SELECT_ITERATOR_ENABLE_TESTS)
  add_subdirectory (test)
endif ()
