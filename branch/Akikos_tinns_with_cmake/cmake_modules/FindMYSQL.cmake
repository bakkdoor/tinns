find_path (MYSQL_INCLUDE_DIR mysql.h /usr/include/mysql)

find_library (MYSQL_LIBRARY NAMES mysqlclient PATHS /usr/lib64 /usr/lib)

if (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
	set (MYSQL_FOUND TRUE)
endif (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

if (MYSQL_FOUND)
	if (NOT mysqlclient_FIND_QUIETLY)
		message (STATUS "Found mysql: ${MYSQL_LIBRARY}")
	endif (NOT mysqlclient_FIND_QUIETLY)
else (MYSQL_FOUND)
	if (mysqlclient_FIND_REQUIRED)
		message (FATAL ERROR "mysql not found")
	endif (mysqlclient_FIND_REQUIRED)
endif (MYSQL_FOUND)
