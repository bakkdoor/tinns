find_path (LUA_INCLUDE_DIR lua.h /usr/include)

find_library (LUA_LIBRARY NAMES lua PATH /usr/lib)

if (LUA_INCLUDE_DIR AND LUA_LIBRARY)
		set (LUA_FOUND TRUE)
endif (LUA_INCLUDE_DIR AND LUA_LIBRARY)

if (LUA_FOUND)
	if (NOT lua_FIND_QUIETLY)
			message (STATUS "Found lua: ${LUA_LIBRARY}")
	endif (NOT lua_FIND_QUIETLY)
else (LUA_FOUND)
		if (LUA_FIND_REQUIRED)
		message (FATAL ERROR "lua not found")
endif (LUA_FIND_REQUIRED)
endif (LUA_FOUND)
