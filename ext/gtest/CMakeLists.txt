cmake_minimum_required(VERSION 2.6)

include(ExternalProject)

ExternalProject_Add(googletest
  SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk
  CMAKE_ARGS -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googletest source_dir)
ExternalProject_Get_Property(googletest binary_dir)

set(GTEST_INCLUDE_DIRS ${source_dir}/include PARENT_SCOPE)
set(GTEST_LIBS_DIR ${binary_dir} PARENT_SCOPE)
set(GTEST_BOTH_LIBS gtest gtest_main PARENT_SCOPE)

