macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif ()
  else ()
    # some GitHub Action windows runners randomly fail with a complaint that Qt6 requires a C++17 compiler
    if (MSVC)
      set (CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to use")
    else ()
      set (CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard to use")
    endif()
    set (CMAKE_CXX_STANDARD_REQUIRED ON)
    if (POLICY CMP0025)
      cmake_policy(SET CMP0025 NEW)
    endif ()
  endif ()
endmacro(use_cxx11)
