################################################################################
#                         COPYRIGHT NOTICE
#                   "Copyright 2023 Nova Biomedical Corporation"
#             This program is the property of Nova Biomedical Corporation
#                 200 Prospect Street, Waltham, MA 02454-9141
#             Any unauthorized use or duplication is prohibited
################################################################################
#
#  Title            -
#  Source Filename  -
#  Author           -
#  Description      -
#
################################################################################


################################################################################
#                      SETUP VARIABLES                                         #
################################################################################
include $(PROJECT_ROOT)/common.mk

# DIRS := $(wildcard */.)
DIRS	:= Libs Plugins Core

################################################################################
#                      TARGET  RECIPES                                         #
################################################################################
.PHONY: all install clean

all:
	@for dir in $(DIRS); do \
		($(MAKE) -C $$dir $@) || exit $$?; \
	done

install:
	@for dir in $(DIRS); do \
		($(MAKE) -C $$dir $@) || exit $$?; \
	done

clean:
	@for dir in $(DIRS); do \
		$(MAKE) -C $$dir $@; \
	done

