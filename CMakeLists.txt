cmake_minimum_required(VERSION 2.6)

include(CheckCXXCompilerFlag)

project(swipl_cpp)

find_program(SWIPL_EXISTS swipl)

if(NOT SWIPL_EXISTS)
  message(SEND_ERROR "Could not find the 'swipl' command on your system, please make sure SWI-Prolog is correctly installed and try again")
  return()
endif()

CHECK_CXX_COMPILER_FLAG("-std=c++11" CXX_SUPPORTS_CPP11)
CHECK_CXX_COMPILER_FLAG("-stdlib=libc++ -std=c++0x" CXX_WLIBCPP_SUPPORTS_CPP0X)
CHECK_CXX_COMPILER_FLAG("-stdlib=libc++ -std=c++11" CXX_WLIBCPP_SUPPORTS_CPP11)

if(CXX_SUPPORTS_CPP0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif(CXX_SUPPORTS_CPP0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(CXX_WLIBCPP_SUPPORTS_CPP0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++0x")
elseif(CXX_WLIBCPP_SUPPORTS_CPP0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11")
else()
  message(SEND_ERROR "C++11 support is necessary. Please update your compiler and try again.")
  return()
endif()

execute_process(
  COMMAND swipl --dump-runtime-variables
  OUTPUT_VARIABLE SWIPL_DUMP
)

CHECK_CXX_COMPILER_FLAG("-std=c++0x" CXX_SUPPORTS_CPP0X)
foreach(S PLARCH;PLBASE;PLCFLAGS;PLLDFLAGS)
  string(REGEX MATCH "${S}=\"([^\n]*)\"" _ ${SWIPL_DUMP})
  set(${S} ${CMAKE_MATCH_1})

  if(NOT DEFINED ${S})
     message(SEND_ERROR "Could not find ${S} in SWI-Prolog runtime dump")
     return()
  endif()

endforeach()

include_directories("${PLBASE}/include" "${swipl_cpp_SOURCE_DIR}/include")

add_definitions(-D_REENTRANT -D__SWI_PROLOG__ -D__SWI_EMBEDDED__)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PLCFLAGS} -Wall")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PLLDFLAGS} -L${PLBASE}/lib/${PLARCH} -lswipl")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

add_library(swiplcpp SHARED src/PrologInterface.cpp)

