#
# TinNS main Makefile
#
# Adapted by Hammag from the Makefile system for Linux kernel.
#

# NOTA: Be carefull no to put extra spaces at EOL when specifing dir path !

# TinNS version (for futur use through $(TINNSRELEASE))
VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 10
EXTRAVERSION =

# Config options
export CONFIG_GAMEMONKEY := n

# Do not change TOPDIR definiton
TOPDIR	:= $(shell /bin/pwd)

# Source code subdirs (libraries directories excluded)
SUBDIRS		= patch info game

# Shortcut targets (when you dont want to make all)
#  generic form is as follows, with tinnslibs rule in first if needed
#SHORTCUT_TARGETn :=[name] : tinnslibs _dir_[sub_dir]
# Implemented for n=0..9 atm. See at the end of the file if more needed

SHORTCUT_TARGET0 :=libs : tinnslibs
SHORTCUT_TARGET1 :=gameserver : tinnslibs _dir_game
SHORTCUT_TARGET2 :=infoserver : tinnslibs _dir_info
SHORTCUT_TARGET3 :=patchserver : tinnslibs _dir_patch

# Directory where we want to build executables in
BINDEST		= $(TOPDIR)/..

# Commun include files stuff
HPATH   	= $(TOPDIR)/include
FINDHPATH	= $(HPATH)
#FINDHPATH	= $(HPATH) $(HPATH)/netcode $(HPATH)/console $(HPATH)/config
#FINDHPATH	= $(HPATH) $(TOPDIR)/game/include $(TOPDIR)/game/decoder

# TinNS libaries stuff
LIBS		= $(TOPDIR)/lib
LIBSSRC 	= common

# Common CFLAGS for all source tree
CPPFLAGS := -I$(HPATH)
CXXFLAGS := $(CPPFLAGS) -pedantic -W -Wall -Wno-long-long -O3 \
   -D_GNU_SOURCE -D_THREAD_SAFE -D_REENTRANT
#CXXFLAGS += -march=$(shell arch)
#CXXFLAGS += -O0 -ggdb
#CXXFLAGS += -Werror

# Common Linker Flags for all source tree (partial linking with LD (ld))
LDFLAGS := -r 

# Common Linker Flags for all source tree (final linking with CXX (g++))
LINKFLAGS := -L$(LIBS)

# specific files to be removed with 'make clean'
CLEAN_FILES = $(BINDEST)/gameserver $(BINDEST)/infoserver \
  $(BINDEST)/patchserver

#
# Install stuf not done yet
#
# INSTALL_PATH := $TOP_DIR/..
# export INSTALL_PATH


#####################################################
#***************************************************#
# No further config should be needed after this line
#  for usual operations
#***************************************************#

TINNSRELEASE=$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)
CFLAGS		:= CXXFLAGS

HOSTCC  	= gcc
HOSTCFLAGS	= -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer

#
# Include the make variables (CC, etc...)
#
CROSS_COMPILE 	=

CC		= $(CROSS_COMPILE)gcc
CXX		= $(CROSS_COMPILE)g++
LD		= $(CROSS_COMPILE)ld
AR		= $(CROSS_COMPILE)ar
STRIP		= $(CROSS_COMPILE)strip
PERL		= perl
DEPDIR		= .dep
export	TOPDIR HPATH HOSTCC HOSTCFLAGS CROSS_COMPILE LD CC \
   CXX AR STRIP MAKE CPPFLAGS CXXFLAGS CFLAGS LDFLAGS LINKFLAGS
export TINNSRELEASE BINDEST LIBS DEPDIR
#
#
#
ALLSUBDIRS	= $(LIBSSRC) $(SUBDIRS)

# directories removed with 'make clean'
#CLEAN_DIRS = 

# files removed with 'make cleaner'
CLEANER_FILES= \
	scripts/lxdialog/*.o scripts/lxdialog/lxdialog \
	.hdepend scripts/mkdep \

# directories removed with 'make cleaner'
#CLEANER_DIRS = \
	include/config


#
# Rules Start
#
all:	do-it-all

do-it-all: tinnslibs tinnssubdirs

#
#
#
tinnslibs : $(patsubst %, _ldir_%, $(LIBSSRC))
$(patsubst %, _ldir_%, $(LIBSSRC)) : dummy 
	$(MAKE) -C $(patsubst _ldir_%, %, $@)


tinnssubdirs: $(patsubst %, _dir_%, $(SUBDIRS)) tinnslibs
$(patsubst %, _dir_%, $(SUBDIRS)) : dummy 
	$(MAKE) -C $(patsubst _dir_%, %, $@)


clean:	
	find . \( -name '*.[oas]' -o -name core -o -name '.*.flags' \) -type f -print \
		| grep -v lxdialog/ | xargs rm -f
	rm -f $(CLEAN_FILES) $(BINDEST)/.*.flags
	rm -rf $(CLEAN_DIRS)

cleaner: clean 
	find . \( -size 0 -o -name .depend \) -type f -print | xargs rm -f
	find . -name .dep -type d -print | xargs rm -Rf
	rm -f $(CLEANER_FILES)
	rm -rf $(CLEANER_DIRS)

distclean: cleaner
	rm -f core `find . \( -not -type d \) -and \
		\( -name '*.orig' -o -name '*.rej' -o -name '*~' \
		-o -name '*.bak' -o -name '#*#' -o -name '.*.orig' \
		-o -name '.*.rej' -o -name '.SUMS' -o -size 0 \) -type f -print` 

backup: cleaner
	cd .. && tar cf - src/ | gzip -9 > tinns-$(TINNSRELEASE)-backup.gz
	sync

sums:
	find . -type f -print | sort | xargs sum > .SUMS

depend dep: 
	@echo This rule is not used anymore. Just do Make

checkincludes:
	find * \( -name '*.h' -o -name '*.cpp' \) -type f -print | sort | xargs $(PERL) -w scripts/checkincludes.pl


include Rules.make

$(SHORTCUT_TARGET0)
$(SHORTCUT_TARGET1)
$(SHORTCUT_TARGET2)
$(SHORTCUT_TARGET3)
$(SHORTCUT_TARGET4)
$(SHORTCUT_TARGET5)
$(SHORTCUT_TARGET6)
$(SHORTCUT_TARGET7)
$(SHORTCUT_TARGET8)
$(SHORTCUT_TARGET9)

