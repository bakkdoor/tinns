add_executable		(pak_decompress pak_decompress.cpp)

target_link_libraries	(pak_decompress ${ZLIB_LIBRARY})
