add_executable        (infoserver accounts.cpp client.cpp globals.cpp infoserver.cpp main.cpp server.cpp sql.cpp)

target_link_libraries (infoserver common ${MYSQL_LIBRARY} ${PCRE_LIBRARY})
