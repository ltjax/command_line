cmake_minimum_required(VERSION 3.2)
project(command_line)


set(MAIN_TARGET command_line)
add_library(${MAIN_TARGET} STATIC
    source/parser.cpp
    source/parser.hpp)


set_property(TARGET ${MAIN_TARGET} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${MAIN_TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)


install(TARGETS ${MAIN_TARGET}
    ARCHIVE DESTINATION lib)

install(FILES source/*.hpp
    DESTINATION include/command_line)
