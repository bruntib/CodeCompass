# Installation directory for libraries
set(INSTALL_LIB_DIR "lib")

# Installation directory for libraries
set(INSTALL_PARSER_DIR "${INSTALL_LIB_DIR}/parserplugin")

# Installation directory for service libraries
set(INSTALL_SERVICE_DIR "${INSTALL_LIB_DIR}/serviceplugin")

#Installation directory for executables
set(INSTALL_BIN_DIR "bin")

set(DATABASE sqlite CACHE STRING "Database type")
string(TOUPPER ${DATABASE} DATABASE_U)

# Odb commands
set(ODB odb)
set(ODBFLAGS
  --std c++11
  --database ${DATABASE}
  --generate-query
  --generate-schema
  --schema-format sql
  --sql-file-suffix -odb
  --default-pointer "std::shared_ptr")

set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -pedantic\
  -std=c++14 \
  -DDATABASE_${DATABASE_U} \
  -DBOOST_LOG_DYN_LINK")

# Cmake module directory (FindOdb, FindThrift etc.)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

# Debug and Release version flags
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
