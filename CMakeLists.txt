cmake_minimum_required(VERSION 2.6)

include(CheckCXXCompilerFlag)

project(swipl_cpp)

find_program(SWIPL_EXISTS swipl)

if(NOT SWIPL_EXISTS)
  message(FATAL_ERROR "Could not find the 'swipl' command on your system, please make sure SWI-Prolog is correctly installed and try again")
  return()
endif()

function(check_cxx11 FLAGS VAR)
  set(OLD_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  set(CMAKE_CXX_FLAGS "${OLD_CMAKE_CXX_FLAGS} -c ${FLAGS}")
  CHECK_CXX_SOURCE_COMPILES(
    "
    #include <initializer_list>
    std::initializer_list<int> list = {1, 2, 3};
    "
    ${VAR}_INIT_LIST
  )
  if(NOT ${VAR}_INIT_LIST)
    return()
  endif()

  CHECK_CXX_SOURCE_COMPILES(
    "
    #include <functional>
    std::function<void (int)> f = [&](int) -> void {};
    "
    ${VAR}_FUNCTIONAL
  )
  if(NOT ${VAR}_FUNCTIONAL)
    return()
  endif()

  set(${VAR} true PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS "${OLD_CMAKE_CXX_FLAGS}")
endfunction()

function(configure_cxx11)
  check_cxx11("-std=c++11" HAS_CPP11)
  if(HAS_CPP11)
    set(CMAKE_CXX_FLAGS "${OLD_CMAKE_CXX_FLAGS} -std=c++11")
    return()
  endif()

  check_cxx11("-std=c++0x" HAS_CPP0X)
  if(HAS_CPP0X)
    set(CMAKE_CXX_FLAGS "${OLD_CMAKE_CXX_FLAGS} -std=c++0x")
    return()
  endif()

  check_cxx11("-std=c++11 -stdlib=libc++" W_LIBCXX_HAS_CPP11)
  if(W_LIBCXX_HAS_CPP11)
    set(CMAKE_CXX_FLAGS "${OLD_CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
    return()
  endif()

  check_cxx11("-std=c++0x -stdlib=libc++" W_LIBCXX_HAS_CPP0X)
  if(W_LIBCXX_HAS_CPP0X)
    set(CMAKE_CXX_FLAGS "${OLD_CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++")
    return()
  endif()

  message(FATAL_ERROR "C++11 support is necessary. Please update your compiler and try again.")
endfunction()

configure_cxx11()

execute_process(
  COMMAND swipl --dump-runtime-variables
  OUTPUT_VARIABLE SWIPL_DUMP
)

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

