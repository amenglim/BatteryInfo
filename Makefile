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
PROJECT_ROOT = $(CURDIR)
export

include common.mk

DIRS := Source

################################################################################
#                      TARGET  RECIPES                                         #
################################################################################
.PHONY: all install clean directories

all: directories
	@$(MAKE) --no-print-directory -C $(DIRS) $@

install:
	@$(MAKE) --no-print-directory -C $(DIRS) $@

clean:
	@$(MAKE) --no-print-directory -C $(DIRS) $@

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(PLUGIN_DIR)
	@mkdir -p $(APP_OBJ_DIR)
	@mkdir -p $(LIB_OBJ_DIR)

