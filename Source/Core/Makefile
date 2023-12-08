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
APP_TARGET	:= Battery_Diagnostic

################################################################################
#                      SETUP VARIABLES                                         #
################################################################################
include $(PROJECT_ROOT)/common.mk

APP_SRC_DIR			:= .
APP_CSRCS			:= $(shell find $(APP_SRC_DIR) -name "*.c")
APP_COBJS			:= $(patsubst %.c, $(APP_OBJ_DIR)/%.o, $(notdir $(APP_CSRCS)))
APP_CXXSRCS			:= $(shell find $(APP_SRC_DIR) -name "*.cpp")
APP_CXXOBJS			:= $(patsubst %.cpp, $(APP_OBJ_DIR)/%.o, $(notdir $(APP_CXXSRCS)))
APP_LIBS			+= -ldl -lpthread -llvgl -ldiag.gui -lgpiodcxx -lgpiod -ldiag.battery -ldiag.audio

################################################################################
#                      TARGET  RECIPES                                         #
################################################################################
.PHONY: all install clean

all: $(BIN_DIR)/$(APP_TARGET)
	@echo -e $(BGreen)$(BIN_DIR)/$(APP_TARGET) COMPLETE$(NC)
	@echo

$(BIN_DIR)/$(APP_TARGET): $(APP_CXXOBJS) $(APP_COBJS)
	@echo -e $(BGreen)Linking $(notdir $@)$(NC)
	$(CXX) $^ --sysroot=$(SYSROOT) $(CXXFLAGS) $(LDFLAGS) $(APP_LIBS) -o "$@"

$(APP_OBJ_DIR)/%.o: %.c
	@echo -e $(BGreen)Compiling $(notdir $<) to $(notdir $@)$(NC)
	$(CC) --sysroot=$(SYSROOT) $(CFLAGS) -c "$<" -o "$@"

$(APP_OBJ_DIR)/%.o: %.cpp
	@echo -e $(BGreen)Compiling $(notdir $<) to $(notdir $@)$(NC)
	$(CXX) --sysroot=$(SYSROOT) $(CXXFLAGS) -c "$<" -o "$@"

install:
	@echo -e $(BBlue)Installing $(APP_TARGET) to $(TARGET_ADDR):$(APP_TARGET_PATH)$(NC)
	scp $(BIN_DIR)/$(APP_TARGET) $(TARGET_ADDR):$(APP_TARGET_PATH)

clean:
	@echo -e $(BBlue)cleaning $(APP_TARGET)$(NC)
	rm -f $(APP_CXXOBJS) $(APP_COBJS) $(BIN_DIR)/$(APP_TARGET)


