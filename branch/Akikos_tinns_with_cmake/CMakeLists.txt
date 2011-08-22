cmake_minimum_required (VERSION 2.8)
project                (TINNS C CXX)

set                    (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

set                    (USE_SQLITE3 false CACHE BOOL "Use SQLITE3 instead of MySQL")

if (USE_SQLITE3)
add_definitions        (-std=c++0x -W -Wall -march=native -mtune=native -O2 -D_THREAD_SAFE -D_REENTRANT -DSQLITE3)
else (USE_SQLITE3)
add_definitions        (-std=c++0x -W -Wall -march=native -mtune=native -O2 -D_THREAD_SAFE -D_REENTRANT)
endif (USE_SQLITE3)

find_package           (RT REQUIRED)
find_package           (PTHREAD REQUIRED)
find_package           (MYSQL REQUIRED)
find_package           (SQLITE3 REQUIRED)
find_package           (PCRE REQUIRED)
find_package           (ZLIB REQUIRED)
find_package           (Lua51 REQUIRED)

add_subdirectory       (server)
add_subdirectory       (tools)
