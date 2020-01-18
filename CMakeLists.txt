cmake_minimum_required(VERSION 3.2)
project(command_line)


set(CMAKE_CXX_STANDARD 17)

option(command_line_TEST "Build Tests" ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(MAIN_TARGET command_line)
add_library(${MAIN_TARGET} STATIC
    source/parser.cpp
    include/command_line/parser.hpp)

target_include_directories(${MAIN_TARGET}
  PUBLIC include/)

set_property(TARGET ${MAIN_TARGET} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${MAIN_TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)

# Only add install directives if needed, i.e. as an installed library
if (command_line_ENABLE_INSTALL)
  install(TARGETS ${MAIN_TARGET}
      ARCHIVE DESTINATION lib)

  install(FILES include/command_line/parser.hpp
      DESTINATION include/command_line)
endif()

if (command_line_TEST)
  add_subdirectory(test)
endif()
