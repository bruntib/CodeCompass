include_directories(
  include
  ${PROJECT_SOURCE_DIR}/model/include
  ${PROJECT_SOURCE_DIR}/util/include
  ${PROJECT_SOURCE_DIR}/parser/include
  ${CMAKE_BINARY_DIR}/model/include)

add_library(searchparser SHARED
  src/searchparser.cpp
  src/ctagsResultParser.cpp)

target_link_libraries(searchparser
  util
  xapian)

target_compile_options(searchparser PUBLIC -Wno-unknown-pragmas)

install(TARGETS searchparser DESTINATION ${INSTALL_PARSER_DIR})
