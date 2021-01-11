include_directories(
  include
  ${CMAKE_CURRENT_BINARY_DIR}/gen-cpp
  ${PROJECT_SOURCE_DIR}/util/include
  ${PROJECT_SOURCE_DIR}/webserver/include
  ${PROJECT_BINARY_DIR}/service/project/gen-cpp
  ${PROJECT_SOURCE_DIR}/service/project/include)

include_directories(SYSTEM
  ${THRIFT_LIBTHRIFT_INCLUDE_DIRS})

add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/gen-cpp/search_constants.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gen-cpp/search_types.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gen-cpp/SearchService.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gen-cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gen-js
  COMMAND
    ${THRIFT_EXECUTABLE} --gen cpp --gen js
      -o ${CMAKE_CURRENT_BINARY_DIR}
      -I ${PROJECT_SOURCE_DIR}/service
      ${CMAKE_CURRENT_SOURCE_DIR}/search.thrift
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/search.thrift
  COMMENT
    "Generating Thrift for search.thrift")

add_library(searchthrift STATIC
  ${CMAKE_CURRENT_BINARY_DIR}/gen-cpp/search_constants.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/gen-cpp/search_types.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/gen-cpp/SearchService.cpp)

target_compile_options(searchthrift PUBLIC -fPIC)

add_dependencies(searchthrift commonthrift projectthrift)

add_library(searchservice SHARED
  src/searchservice.cpp
  src/plugin.cpp)

target_compile_options(searchservice PUBLIC -Wno-unknown-pragmas)

target_link_libraries(searchservice
  searchthrift
  projectservice
  xapian
  ${THRIFT_LIBTHRIFT_LIBRARIES})

install(TARGETS searchservice DESTINATION ${INSTALL_SERVICE_DIR})
install_js_thrift()
