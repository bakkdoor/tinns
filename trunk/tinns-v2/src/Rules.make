#
# This file contains rules which are shared between multiple Makefiles.
#
# Adapted by Hammag from the Makefile system for Linux kernel.
#
#####################################################
#***************************************************#
# This file is not intended to be modified
#***************************************************#

#
# False targets.
#
.PHONY: dummy

#
# Special variables which should not be exported
#
unexport EXTRA_CXXFLAGS
unexport EXTRA_LINKFLAGS
unexport EXTRA_LDFLAGS
unexport EXTRA_ARFLAGS
unexport SUBDIRS
unexport SUB_DIRS
unexport ALL_SUB_DIRS
unexport B_TARGET
unexport B_REALTARGET
unexport O_TARGET
unexport L_TARGET
unexport L_REALTARGET

unexport obj-y
unexport obj-n
unexport obj-
unexport subdir-y
unexport subdir-n
unexport subdir-

comma	:= ,


SUB_DIRS	:= $(subdir-y)
#ALL_SUB_DIRS	:= $(sort $(subdir-y) $(subdir-n) $(subdir-))
ALL_SUB_DIRS	:= $(sort $(subdir-y))
ifdef B_TARGET
   ifdef BINDEST
      B_REALTARGET := $(strip $(BINDEST))/$(B_TARGET)
   else
      B_REALTARGET := $(B_TARGET)   
   endif
endif
L_REALTARGET 	:= $(patsubst %,$(LIBS)/lib%.a,$(L_TARGET))
ALL_TARGETS	:= $(B_REALTARGET) $(O_TARGET) $(L_REALTARGET)

#
# Get things started.
#
first_rule: sub_dirs
	$(if $(strip $(ALL_TARGETS)),@$(MAKE) --no-print-directory all_targets,)

#
# Common rules
#
%.o: %.cpp
	@if [ ! -d .dep ]; then mkdir .dep ; fi
	$(CXX) -MD -MP $(CXXFLAGS) $(EXTRA_CXXFLAGS) $(CXXFLAGS_$@) -c $< -o $@
	@ ( \
	    egrep -v "/usr/include|/usr/lib|^$$" $(@:.o=.d) > $(DEPDIR)/$(@:.o=.dep); \
	    rm -f $(@:.o=.d); \
	)
	@ ( \
	    echo 'ifeq ($(strip $(subst $(comma),:,$(CXXFLAGS) $(EXTRA_CXXFLAGS) $(CXXFLAGS_$@))),$$(strip $$(subst $$(comma),:,$$(CXXFLAGS) $$(EXTRA_CXXFLAGS) $$(CXXFLAGS_$@))))' ; \
	    echo 'FILES_FLAGS_UP_TO_DATE += $@' ; \
	    echo 'endif' \
	) > $(dir $@)/.$(notdir $@).flags

#
#
#
all_targets: $(ALL_TARGETS)

#
# Rule to compile a set of .o files into one executable file
#
ifdef B_REALTARGET

FULLEXT_LINKFLAGS := $(EXTRA_LINKFLAGS) $(patsubst %,-l%,$(LINK_TINNSLIBS))
LIBDEP := $(patsubst %,$(LIBS)/lib%.a,$(LINK_TINNSLIBS))

$(B_REALTARGET): $(obj-y) $(LIBDEP)
	@rm -f $@
    ifneq "$(strip $(obj-y))" ""
	$(CXX) -o $@ $(filter $(obj-y), $^) $(LINKFLAGS) $(FULLEXT_LINKFLAGS)
    else
	@echo No object files to build $@ !!!
    endif
	@ ( \
	    echo 'ifeq ($(strip $(subst $(comma),:,$(LINKFLAGS) $(FULLEXT_LINKFLAGS) $(obj-y))),$$(strip $$(subst $$(comma),:,$$(LINKFLAGS) $$(FULLEXT_LINKFLAGS) $$(obj-y))))' ; \
	    echo 'FILES_FLAGS_UP_TO_DATE += $@' ; \
	    echo 'endif' \
	) > $(dir $@)/.$(notdir $@).flags
endif # B_REALTARGET

#
# Rule to compile a set of .o files into one .o file
#
ifdef O_TARGET
$(O_TARGET): $(obj-y)
	rm -f $@
    ifneq "$(strip $(obj-y))" ""
	$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $@ $(filter $(obj-y), $^)
    else
	@echo No object files to build $@ !!!
#	$(AR) rcs $@
    endif
	@ ( \
	    echo 'ifeq ($(strip $(subst $(comma),:,$(LDFLAGS) $(EXTRA_LDFLAGS) $(obj-y))),$$(strip $$(subst $$(comma),:,$$(LDFLAGS) $$(EXTRA_LDFLAGS) $$(obj-y))))' ; \
	    echo 'FILES_FLAGS_UP_TO_DATE += $@' ; \
	    echo 'endif' \
	) > $(dir $@)/.$(notdir $@).flags
endif # O_TARGET

#
# Rule to compile a set of .o files into one .a file, creating,
#  adding or updating it
#
ifdef L_REALTARGET
$(L_REALTARGET): $(obj-y)
#	rm -f $@
	$(if $(filter $(obj-y),$?),$(AR) $(EXTRA_ARFLAGS) rcs $@ $(filter $(obj-y),$?),)
	@ ( \
	    echo 'ifeq ($(strip $(subst $(comma),:,$(EXTRA_ARFLAGS) $(obj-y))),$$(strip $$(subst $$(comma),:,$$(EXTRA_ARFLAGS) $$(obj-y))))' ; \
	    echo 'FILES_FLAGS_UP_TO_DATE += $@' ; \
	    echo 'endif' \
	) > $(dir $@)/.$(notdir $@).flags
endif #L_REALTARGET

#
# This make dependencies quickly
#
fastdep: dummy
	@$(TOPDIR)/scripts/mkdep $(CXXFLAGS) $(EXTRA_CXXFLAGS) -- $(wildcard *.h) $(wildcard *.cpp) > .depend
ifdef ALL_SUB_DIRS
	@$(MAKE) $(patsubst %,_sfdep_%,$(ALL_SUB_DIRS)) _FASTDEP_ALL_SUB_DIRS="$(ALL_SUB_DIRS)"
endif

ifdef _FASTDEP_ALL_SUB_DIRS
$(patsubst %,_sfdep_%,$(_FASTDEP_ALL_SUB_DIRS)):
	@$(MAKE) -C $(patsubst _sfdep_%,%,$@) fastdep
endif

	
#
# A rule to make subdirectories
#
subdir-list = $(sort $(patsubst %,_subdir_%,$(SUB_DIRS)))
sub_dirs: dummy $(subdir-list)

ifdef SUB_DIRS
$(subdir-list) : dummy
	$(MAKE) -C $(patsubst _subdir_%,%,$@)
endif

#
# A rule to do nothing
#
dummy:
#
# This is useful for testing
#
script:
	$(SCRIPT)

dep: fastdep 

#
# include dependency files if they exist
#
ifneq ($(wildcard $DEPDIR)),)
-include $(patsubst %.o,$(DEPDIR)/%.dep,$(obj-y))
endif

ifneq ($(wildcard $(TOPDIR)/.hdepend),)
include $(TOPDIR)/.hdepend
endif

#
# Find files whose flags have changed and force recompilation.
# For safety, this works in the converse direction:
#   every file is forced, except those whose flags are positively up-to-date.
#
FILES_FLAGS_UP_TO_DATE :=

# For use in expunging commas from flags, which mung our checking.
comma = ,

FILES_FLAGS_EXIST := $(wildcard .*.flags)
ifdef B_TARGET
   ifneq ($(B_REALTARGET),$(B_TARGET))
      FILES_FLAGS_EXIST += $(wildcard $(dir $(B_REALTARGET))/.$(notdir $(B_REALTARGET)).flags)
      FILESTEST := $(dir $(B_REALTARGET))/.$(notdir $(B_REALTARGET)).flags
   endif
endif
ifdef L_REALTARGET
   FILES_FLAGS_EXIST += $(wildcard $(dir $(L_REALTARGET))/.$(notdir $(L_REALTARGET)).flags)
endif

ifneq ($(FILES_FLAGS_EXIST),)
include $(FILES_FLAGS_EXIST)
endif

FILES_FLAGS_CHANGED := $(strip \
    $(filter-out $(FILES_FLAGS_UP_TO_DATE), \
	$(obj-y) $(ALL_TARGETS) \
	))

ifneq ($(FILES_FLAGS_CHANGED),)
$(FILES_FLAGS_CHANGED): dummy
endif

