find_path (PCRE_INCLUDE_DIR pcre.h /usr/include)

find_library (PCRE_LIBRARY NAMES pcre PATH /usr/lib)

if (PCRE_INCLUDE_DIR AND PCRE_LIBRARY)
	set (PCRE_FOUND TRUE)
endif (PCRE_INCLUDE_DIR AND PCRE_LIBRARY)

if (PCRE_FOUND)
	if (NOT pcre_FIND_QUIETLY)
		message (STATUS "Found pcre: ${PCRE_LIBRARY}")
	endif (NOT pcre_FIND_QUIETLY)
else (PCRE_FOUND)
	if (PCRE_FIND_REQUIRED)
		message (FATAL ERROR "pcre not found")
	endif (PCRE_FIND_REQUIRED)
endif (PCRE_FOUND)
