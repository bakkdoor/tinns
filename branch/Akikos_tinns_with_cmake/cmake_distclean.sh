#!/bin/bash

rm -rf CMakeCache.txt
rm -rf cmake_install.cmake
rm -rf Makefile
rm -rf CMakeFiles/
rm -rf tools/cmake_install.cmake
rm -rf tools/Makefile
rm -rf tools/CMakeFiles/
rm -rf tools/pak_decompress/cmake_install.cmake
rm -rf tools/pak_decompress/Makefile
rm -rf tools/pak_decompress/CMakeFiles/
rm -rf tools/vfs_viewer/cmake_install.cmake
rm -rf tools/vfs_viewer/Makefile
rm -rf tools/vfs_viewer/CMakeFiles/
rm -rf server/cmake_install.cmake
rm -rf server/Makefile server/CMakeFiles/
rm -rf server/src/cmake_install.cmake
rm -rf server/src/Makefile server/src/CMakeFiles/
rm -rf server/src/common/cmake_install.cmake
rm -rf server/src/common/Makefile
rm -rf server/src/common/CMakeFiles/
rm -rf server/src/common/config/cmake_install.cmake
rm -rf server/src/common/config/Makefile
rm -rf server/src/common/config/CMakeFiles/
rm -rf server/src/common/console/cmake_install.cmake
rm -rf server/src/common/console/Makefile
rm -rf server/src/common/console/CMakeFiles/
rm -rf server/src/common/filesystem/cmake_install.cmake
rm -rf server/src/common/filesystem/Makefile
rm -rf server/src/common/filesystem/CMakeFiles/
rm -rf server/src/common/misc/cmake_install.cmake
rm -rf server/src/common/misc/Makefile
rm -rf server/src/common/misc/CMakeFiles/
rm -rf server/src/common/netcode/cmake_install.cmake
rm -rf server/src/common/netcode/Makefile
rm -rf server/src/common/netcode/CMakeFiles/
rm -rf server/src/common/regex/cmake_install.cmake
rm -rf server/src/common/regex/Makefile
rm -rf server/src/common/regex/CMakeFiles/
rm -rf server/src/dev-tools/cmake_install.cmake
rm -rf server/src/dev-tools/Makefile
rm -rf server/src/dev-tools/CMakeFiles/
rm -rf server/src/game/cmake_install.cmake
rm -rf server/src/game/Makefile
rm -rf server/src/game/CMakeFiles/
rm -rf server/src/game/decoder/cmake_install.cmake
rm -rf server/src/game/decoder/Makefile
rm -rf server/src/game/decoder/CMakeFiles/
rm -rf server/src/game/def/cmake_install.cmake
rm -rf server/src/game/def/Makefile
rm -rf server/src/game/def/CMakeFiles/
rm -rf server/src/game/gamecommands/cmake_install.cmake
rm -rf server/src/game/gamecommands/Makefile
rm -rf server/src/game/gamecommands/CMakeFiles/
rm -rf server/src/info/cmake_install.cmake
rm -rf server/src/info/Makefile
rm -rf server/src/info/CMakeFiles/
rm -rf server/src/patch/cmake_install.cmake
rm -rf server/src/patch/Makefile
rm -rf server/src/patch/CMakeFiles/

# code counter

LOG="/tmp/log.$$"
LINES=0
C=0

find|grep "\.c$" >$LOG
find|grep "\.cpp$" >>$LOG
find|grep "\.h$" >>$LOG
find|grep "\.hpp$" >>$LOG
find|grep "\.sh$" >>$LOG
find|grep "\.S$" >>$LOG
find|grep "install-script$" >>$LOG
find|grep "bar$" >>$LOG
find|grep "colors$" >>$LOG
find|grep "function$" >>$LOG
while read L
do
    C=$((C+1))
    W=$(cat $L|wc -l)
    S=$(cat $L|wc -c)
    SIZE=$((SIZE+S))
    LINES=$((LINES+W))
done <$LOG
rm $LOG

echo
echo "--- REPOSITORY SUMMARY ---"
echo "source files:  $C (c/cpp/h/hpp/sh/S)"
echo "lines of code: $LINES"
echo "size of code:  $SIZE bytes"
echo
