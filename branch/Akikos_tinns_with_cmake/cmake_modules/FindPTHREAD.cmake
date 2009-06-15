find_path (PTHREAD_INCLUDE_DIR pthread.h /usr/include)

find_library (PTHREAD_LIBRARY NAMES pthread PATH /usr/lib)

if (PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY)
	set (PTHREAD_FOUND TRUE)
endif (PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY)

if (PTHREAD_FOUND)
	if (NOT pthread_FIND_QUIETLY)
		message (STATUS "Found pthread: ${PTHREAD_LIBRARY}")
	endif (NOT pthread_FIND_QUIETLY)
else (PTHREAD_FOUND)
	if (pthread_FIND_REQUIRED)
		message (FATAL ERROR "pthread not found")
	endif (pthread_FIND_REQUIRED)
endif (PTHREAD_FOUND)
