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
#                      TARGETS                                                 #
################################################################################
LIB_NAME			:= gui

################################################################################
#                      SETUP VARIABLES                                         #
################################################################################
include $(PROJECT_ROOT)/common.mk

LIB_TARGET			:= $(LIB_PREFIX)$(LIB_NAME)$(LIB_SUFFIX)
LIB_SRC_DIR			:= .
LIB_CSRCS			:= $(shell find $(LIB_SRC_DIR) -name "*.c")
LIB_COBJS			:= $(patsubst %.c, $(LIB_OBJ_DIR)/%.o, $(notdir $(LIB_CSRCS)))
LIB_CXXSRCS			:= $(shell find $(LIB_SRC_DIR) -name "*.cpp")
LIB_CXXOBJS			:= $(patsubst %.cpp, $(LIB_OBJ_DIR)/%.o, $(notdir $(LIB_CXXSRCS)))
LIB_LIBS			:= -llvgl

################################################################################
#                      TARGET  RECIPES                                         #
################################################################################
.PHONY: all install clean

all: $(LIB_DIR)/$(LIB_TARGET)
	@echo -e $(BGreen)$(LIB_TARGET) COMPLETE$(NC)
	@echo

$(LIB_DIR)/$(LIB_TARGET): $(LIB_CXXOBJS) $(LIB_COBJS)
	@echo -e $(BGreen)Linking $(notdir $@)$(NC)
	$(CXX) $^ --sysroot=$(SYSROOT) $(LIB_CXXFLAGS) $(LDFLAGS) $(LIB_LIBS) -o "$@"

$(LIB_OBJ_DIR)/%.o: %.c
	@echo -e $(BGreen)Compiling $(notdir $<) to $(notdir $@)$(NC)
	$(CC) --sysroot=$(SYSROOT) $(LIB_CFLAGS) -c "$<" -o "$@"

$(LIB_OBJ_DIR)/%.o: %.cpp
	@echo -e $(BGreen)Compiling $(notdir $<) to $(notdir $@)$(NC)
	$(CXX) --sysroot=$(SYSROOT) $(LIB_CXXFLAGS) -c "$<" -o "$@"

install:
	@echo -e $(BBlue)Installing $(LIB_DIR)/$(LIB_TARGET) to $(TARGET_ADDR):$(LIB_TARGET_PATH)$(NC)
	scp $(LIB_DIR)/$(LIB_TARGET) $(TARGET_ADDR):$(LIB_TARGET_PATH)

clean:
	@echo -e $(BBlue)cleaning $(LIB_NAME)$(NC)
	rm -f $(LIB_COBJS) $(LIB_CXXOBJS) $(LIB_DIR)/$(LIB_TARGET)

