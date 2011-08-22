cmake_minimum_required (VERSION 2.6)
project                (TINNS C CXX)

set                    (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

set                    (USE_SQLITE3 false CACHE BOOL "Use SQLITE3 instead of MySQL")
set                    (USE_MYSQL_INCLUDE_DIR false CACHE BOOL "MySQL includes in ../include/mysql/")

add_definitions        (-std=c++0x -W -Wall -march=native -mtune=native -O2 -D_THREAD_SAFE -D_REENTRANT)

if (USE_SQLITE3)
add_definitions        (-DSQLITE3)
endif (USE_SQLITE3)

if (USE_MYSQL_INCLUDE_DIR)
add_definitions        (-DMYSQL_INC_DIR)
endif (USE_MYSQL_INCLUDE_DIR)

find_package           (RT REQUIRED)
find_package           (PTHREAD REQUIRED)
find_package           (MYSQL REQUIRED)
find_package           (SQLITE3 REQUIRED)
find_package           (PCRE REQUIRED)
find_package           (ZLIB REQUIRED)
find_package           (Lua51 REQUIRED)

add_subdirectory       (server)
add_subdirectory       (tools)
