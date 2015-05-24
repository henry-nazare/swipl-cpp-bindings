include(CheckCXXCompilerFlag)

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

function(find_cxx11 FLAGS)
  check_cxx11("-std=c++11" HAS_CPP11)
  if(HAS_CPP11)
    set(${FLAGS} "-std=c++11" PARENT_SCOPE)
    return()
  endif()

  check_cxx11("-std=c++0x" HAS_CPP0X)
  if(HAS_CPP0X)
    set(${FLAGS} "-std=c++0x" PARENT_SCOPE)
    return()
  endif()

  check_cxx11("-std=c++11 -stdlib=libc++" W_LIBCXX_HAS_CPP11)
  if(W_LIBCXX_HAS_CPP11)
    set(${FLAGS} "-std=c++11 -stdlib=libc++" PARENT_SCOPE)
    return()
  endif()

  check_cxx11("-std=c++0x -stdlib=libc++" W_LIBCXX_HAS_CPP0X)
  if(W_LIBCXX_HAS_CPP0X)
    set(${FLAGS} "-std=c++0x -stdlib=libc++" PARENT_SCOPE)
    return()
  endif()
endfunction()

find_cxx11(SPC_CPP11_FLAGS)

