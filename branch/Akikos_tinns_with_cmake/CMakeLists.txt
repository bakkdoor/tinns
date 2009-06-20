cmake_minimum_required	(VERSION 2.4)
project			(TINNS)

set			(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

add_definitions		(-W -Wall -O3 -fomit-frame-pointer -static-libgcc
			-D_GNU_SOURCE -D_THREAD_SAFE -D_REENTRANT)

find_package		(PTHREAD REQUIRED)
find_package		(MYSQL REQUIRED)
find_package		(PCRE REQUIRED)
find_package		(ZLIB REQUIRED)
find_package		(RT REQUIRED)

#add_subdirectory	(docs)
add_subdirectory	(server)
add_subdirectory	(tools)