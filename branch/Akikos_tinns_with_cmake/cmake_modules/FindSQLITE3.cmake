find_path (SQLITE3_INCLUDE_DIR sqlite3.h /usr/include)

find_library (SQLITE3_LIBRARY NAMES sqlite3 PATHS /usr/lib64 /usr/lib)

if (SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
	set (SQLITE3_FOUND TRUE)
endif (SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)

if (SQLITE3_FOUND)
	if (NOT sqlite3_FIND_QUIETLY)
		message (STATUS "Found sqlite3: ${SQLITE3_LIBRARY}")
	endif (NOT sqlite3_FIND_QUIETLY)
else (SQLITE3_FOUND)
	if (SQLITE3_FIND_REQUIRED)
		message (FATAL ERROR "sqlite3 not found")
	endif (sqlite3_FIND_REQUIRED)
endif (SQLITE3_FOUND)
