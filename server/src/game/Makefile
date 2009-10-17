#
# TinNS per-directory Makefile
#
# Adapted by Hammag from the Makefile system for Linux kernel.
#
# 14 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
# 

######################################################
# Target for this directory and its sub-dirs
######################################################
# Case 1 : target is an executable named as defined
B_TARGET := gameserver 

# Case 2 : target is a TinNS lib name
#  (use short name, as in -l linker option) 
#L_TARGET := tinns

# Case 3 (usual): objects shall be made available to the parent dir
#  The following line will set that automatically 
# (Should match dir name + .o suffix)
#O_TARGET := $(addsuffix .o,$(notdir $(shell /bin/pwd)))
# Manualy define this only if you need to force the compiled obj name
#O_TARGET := game.o


######################################################
# Local flags
######################################################
# local dir CXX Flags
EXTRA_CXXFLAGS := -I/usr/include/mysql -Iinclude 

# per-object CXX Flags
#CXXFLAGS_funcdef2.o := -g

# local dir Linker Flags (for intermediate .o linking)
#EXTRA_LDFLAGS := 

# any tinns Lib used (short name, as in -l linker option)
LINK_TINNSLIBS := tinns

# local dir Linker Flags (for final executable linking)
#EXTRA_LINKFLAGS := -lpthread -lm -lcrypt -lrt -L"/usr/lib/mysql" -lmysqlclient -lz
EXTRA_LINKFLAGS := -lrt -L"/usr/lib/mysql" -lmysqlclient -lz -lpcre -llua


#####################################################
# Subdirectories
#####################################################
subdir-y := decoder def gamecommands
#subdir-$(CONFIG_GAMEMONKEY)	+= gamemonkey


#####################################################
#***************************************************#
# No further config should be needed after this line
#***************************************************#
ifdef TOPDIR

# build list of objects ib loca ldir
obj-y := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
# add final object from each subdir
obj-y += $(join $(subdir-y),$(subdir-y:%=/%.o))

include $(TOPDIR)/Rules.make

else #TOPDIR undef, Makefile called from non-top dir

# Memorize the first calling dir in case we wanted to
# lauch local actions from top Makefile
  ifndef FIRSTDIR
    FIRSTDIR :=$(CURDIR)
    export FIRSTDIR
  endif

uptoparent : 
	$(MAKE) -C .. $(MAKECMDGOALS)

.DEFAULT :
	$(MAKE) -C .. $(MAKECMDGOALS)

endif #TOPDIR

