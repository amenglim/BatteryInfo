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

################################################################################
#             point LVGL_BUILD to your local LVGL build tree                   #
################################################################################
LVGL_BUILD			:= /home/amenglim/builds/lvgl_build_tree/lvgl-7.10.1

BUILD_DIR			:= $(PROJECT_ROOT)/Build
BIN_DIR				:= $(BUILD_DIR)/bin
LIB_DIR				:= $(BUILD_DIR)/lib
PLUGIN_DIR			:= $(BUILD_DIR)/plugins
APP_OBJ_DIR			:= $(BUILD_DIR)/objs/app
LIB_OBJ_DIR			:= $(BUILD_DIR)/objs/lib
LVGL_DIRS			:= $(shell find $(LVGL_BUILD) -type d)
LVGL_INCLUDES		:= $(patsubst %, -I%, $(LVGL_DIRS))
INCLUDES            += -I$(PROJECT_ROOT)/Include -I$(SYSROOT)/usr/include $(LVGL_INCLUDES)
SYSROOT				:= /opt/nova/toolchain/arm-buildroot-linux-gnueabihf/sysroot
CROSS_COMPILER		:= arm-none-linux-gnueabihf-
CXX					:= $(CROSS_COMPILER)g++
CC					:= $(CROSS_COMPILER)gcc
OBJCOPY				:= $(CROSS_COMPILER)objcopy
CXXFLAGS			= -Wall -Wextra $(INCLUDES) -fpie -s
CFLAGS				= -Wall -Wextra $(INCLUDES) -fpie -s
LIB_CXXFLAGS		= -Wall -Wextra $(INCLUDES) -fpic -shared
LIB_CFLAGS			= -Wall -Wextra $(INCLUDES) -fpic -shared
LDFLAGS				= -s -L$(LVGL_BUILD)/lib -L$(SYSROOT)/lib -L$(SYSROOT)/usr/lib -L$(LIB_DIR)
LIB_PREFIX			:= libdiag.
LIB_SUFFIX			:= .so
PLUGIN_SUFFIX		:= .pi.so
NC					:= '\033[0m'
BGreen				:= '\033[1;32m'
BBlue				:= '\033[1;34m'

################################################################################
#                           Install variables                                  #
#             Add your dev board's IP address here and uncomment               #
################################################################################
#TARGET_ADDR			:= root@192.168.1.43
TARGET_ADDR			:= root@10.0.0.2

#APP_TARGET_PATH		:= /usr/nova/diags/bin
#LIB_TARGET_PATH		:= /usr/lib
#PLUGIN_TARGET_PATH	:= /usr/nova/diags/plugins
#APP_TARGET_PATH		:= /var/nova/battery_info/usr/nova/battery_info/bin
#LIB_TARGET_PATH		:= /var/nova/battery_info/usr/lib
#PLUGIN_TARGET_PATH	:= /var/nova/battery_info/usr/nova/battery_info/plugins
APP_TARGET_PATH		:= /usr/nova/battery_info/bin
LIB_TARGET_PATH		:= /usr/lib
PLUGIN_TARGET_PATH	:= /usr/nova/battery_info/plugins

#home root@192.168.1.43
#work root@192.168.98.31
