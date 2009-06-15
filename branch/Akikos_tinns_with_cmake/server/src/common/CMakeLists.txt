#add_subdirectory	(config)
#add_subdirectory	(console)
#add_subdirectory	(filesystem)
#add_subdirectory	(misc)
#add_subdirectory	(netcode)
#add_subdirectory	(regex)

add_library		(tinns config/config.cpp console/console.cpp
			filesystem/filesystem.cpp misc/misc.cpp
			netcode/serversocket.cpp netcode/udpmanager.cpp
			netcode/message.cpp netcode/connection-tcp.cpp
			netcode/connection-udp.cpp regex/regex++.cpp)
