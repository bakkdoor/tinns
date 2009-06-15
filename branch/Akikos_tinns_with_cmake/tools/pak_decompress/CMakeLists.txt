add_executable		(pak_decompress pak_decompress.cpp)

set_target_properties	(pak_decompress PROPERTIES
			LINK_FLAGS "-lz")
