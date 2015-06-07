cmake_minimum_required(VERSION 2.6)

include(ExternalProject)

ExternalProject_Add(googlemock
  SVN_REPOSITORY http://googlemock.googlecode.com/svn/trunk
  CMAKE_ARGS -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googlemock source_dir)
ExternalProject_Get_Property(googlemock binary_dir)

set(GMOCK_INCLUDE_DIRS ${source_dir}/include PARENT_SCOPE)
set(GMOCK_LIBS_DIR ${binary_dir} PARENT_SCOPE)
set(GMOCK_BOTH_LIBS gmock gmock_main PARENT_SCOPE)

