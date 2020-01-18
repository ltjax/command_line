
set(TARGET_NAME command_line_tests)
add_executable(${TARGET_NAME}
  main.cpp
  parser.t.cpp)

target_link_libraries(${TARGET_NAME}
  PUBLIC command_line
  PUBLIC CONAN_PKG::Catch2)