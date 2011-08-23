add_executable        (patchserver client.cpp globals.cpp patchserver.cpp main.cpp server.cpp)

target_link_libraries (patchserver common ${PCRE_LIBRARY} ${ZLIB_LIBRARY})
