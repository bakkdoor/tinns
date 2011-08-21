find_path (RT_INCLUDE_DIR time.h /usr/include)

find_library (RT_LIBRARY NAMES rt PATHS /usr/lib64 /usr/lib)

if (RT_INCLUDE_DIR AND RT_LIBRARY)
	set (RT_FOUND TRUE)
endif (RT_INCLUDE_DIR AND RT_LIBRARY)

if (RT_FOUND)
	if (NOT rt_FIND_QUIETLY)
		message (STATUS "Found librt: ${RT_LIBRARY}")
	endif (NOT rt_FIND_QUIETLY)
else (RT_FOUND)
	if (RT_FIND_REQUIRED)
		message (FATAL ERROR "librt not found")
	endif (RT_FIND_REQUIRED)
endif (RT_FOUND)
