###################################################################
# About the application name and path
###################################################################

# Application name, can be suffixed by the SDK
APP_NAME ?= pin
# application build directory name
DIR_NAME := pin

# project root directory, relative to app dir
PROJ_FILES = ../../

# binary, hex and elf file names
BIN_NAME = $(APP_NAME).bin
HEX_NAME = $(APP_NAME).hex
ELF_NAME = $(APP_NAME).elf

# SDK helper Makefiles inclusion
-include $(PROJ_FILES)/Makefile.conf
-include $(PROJ_FILES)/Makefile.gen

# application build directory, relative to the SDK BUILD_DIR environment
# variable.
APP_BUILD_DIR = $(BUILD_DIR)/apps/$(DIR_NAME)

###################################################################
# About the compilation flags
###################################################################

# Application CFLAGS, first yours...
CFLAGS += -Isrc/ -Iinc/ -MMD -MP -Os
# and the SDK ones
CFLAGS += $(APPS_CFLAGS)

###################################################################
# About the link step
###################################################################

# the layout file, must be named 'myapp.fw1.ld' for default. is overriden by
# the SDK in multi-bank case.
# this is an option of the linker.
EXTRA_LDFLAGS ?= -Tpin.fw1.ld

# linker options to add the layout file
LDFLAGS += $(EXTRA_LDFLAGS) -L$(APP_BUILD_DIR)

# generic linker options
LDFLAGS += -fno-builtin -nostdlib -nostartfiles $(AFLAGS_GCC) -Wl,-Map=$(APP_BUILD_DIR)/$(APP_NAME).map

# project's library you whish to use...
ifeq (y,$(CONFIG_APP_PIN_INPUT_SCREEN))
LD_LIBS += -lspi -lad7843 -lili9341 -lgui -lstd -lfirmware
endif
ifeq (y,$(CONFIG_APP_PIN_MOCKUP_SHOW_MENU))
LD_LIBS += -lspi -lad7843 -lili9341 -lgui -lstd -lfirmware
endif
ifeq (y,$(CONFIG_APP_PIN_INPUT_USART))
LD_LIBS += -lstd -lconsole -lusart -lfirmware
endif
ifeq (y,$(CONFIG_APP_PIN_INPUT_MOCKUP))
LD_LIBS += -lstd -lfirmware
endif

###################################################################
# okay let's list our source files and generated files now
###################################################################

CSRC_DIR = src
SRC = $(wildcard $(CSRC_DIR)/*.c)
OBJ = $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(SRC))
DEP = $(OBJ:.o=.d)

# the output directories, that will be deleted by the distclean target
OUT_DIRS = $(dir $(OBJ))

# the ldcript file generated by the SDK
LDSCRIPT_NAME = $(APP_BUILD_DIR)/$(APP_NAME).ld

# file to (dist)clean

# first, objects and compilation related
TODEL_CLEAN += $(OBJ) $(DEP)

# the overall target content
TODEL_DISTCLEAN += $(APP_BUILD_DIR) $(LDSCRIPT_NAME)

.PHONY: app

###################################################################
# build targets (driver, core, SoC, Board... and local)
###################################################################

# all (default) build the app
all: $(APP_BUILD_DIR) alldeps app

############################################################
# eplicit dependency on the application libs and drivers
# compiling the application requires the compilation of its
# dependencies
#
## library dependencies
LIBDEP := $(BUILD_DIR)/libs/libstd/libstd.a
ifeq (y,$(CONFIG_APP_PIN_INPUT_SCREEN))
LIBDEP += $(BUILD_DIR)/libs/libgui/libgui.a
endif
ifeq (y,$(CONFIG_APP_PIN_MOCKUP_SHOW_MENU))
LIBDEP += $(BUILD_DIR)/libs/libgui/libgui.a
endif
ifeq (y,$(CONFIG_APP_PIN_INPUT_USART))
LIBDEP += $(BUILD_DIR)/libs/libconsole/libconsole.a
endif
LIBDEP += $(BUILD_DIR)/libs/libfirmware/libfirmware.a

libdep: $(LIBDEP)

$(LIBDEP):
	$(Q)$(MAKE) -C $(PROJ_FILES)libs/$(patsubst lib%.a,%,$(notdir $@))


# drivers dependencies
SOCDRVDEP :=

ifeq (y,$(CONFIG_APP_PIN_INPUT_SCREEN))
SOCDRVDEP += $(BUILD_DIR)/drivers/libspi/libspi.a
endif
ifeq (y,$(CONFIG_APP_PIN_MOCKUP_SHOW_MENU))
SOCDRVDEP += $(BUILD_DIR)/drivers/libspi/libspi.a
endif
ifeq (y,$(CONFIG_APP_PIN_INPUT_USART))
SOCDRVDEP += $(BUILD_DIR)/drivers/libusart/libusart.a
endif

socdrvdep: $(SOCDRVDEP)

$(SOCDRVDEP):
	$(Q)$(MAKE) -C $(PROJ_FILES)drivers/socs/$(SOC)/$(patsubst lib%.a,%,$(notdir $@))

# board drivers dependencies
BRDDRVDEP    :=

ifeq (y,$(CONFIG_APP_PIN_INPUT_SCREEN))
BRDDRVDEP += $(BUILD_DIR)/drivers/libad7843/libad7843.a
BRDDRVDEP += $(BUILD_DIR)/drivers/libili9341/libili9341.a
endif
ifeq (y,$(CONFIG_APP_PIN_MOCKUP_SHOW_MENU))
BRDDRVDEP += $(BUILD_DIR)/drivers/libad7843/libad7843.a
BRDDRVDEP += $(BUILD_DIR)/drivers/libili9341/libili9341.a
endif

brddrvdep: $(BRDDRVDEP)

$(BRDDRVDEP):
	$(Q)$(MAKE) -C $(PROJ_FILES)drivers/boards/$(BOARD)/$(patsubst lib%.a,%,$(notdir $@))

# external dependencies
EXTDEP    :=

extdep: $(EXTDEP)

$(EXTDEP):
	$(Q)$(MAKE) -C $(PROJ_FILES)externals


alldeps: libdep socdrvdep brddrvdep extdep

##########################################################


# app build the hex and elf binaries
app: $(APP_BUILD_DIR)/$(ELF_NAME) $(APP_BUILD_DIR)/$(HEX_NAME)

# objet files and dependencies
$(APP_BUILD_DIR)/%.o: %.c
	$(call if_changed,cc_o_c)

# ELF file dependencies. libs are build separately and before.
# Be sure to add the libs to your config file!
$(APP_BUILD_DIR)/$(ELF_NAME): $(OBJ)
	$(call if_changed,link_o_target)

# same for hex
$(APP_BUILD_DIR)/$(HEX_NAME): $(APP_BUILD_DIR)/$(ELF_NAME)
	$(call if_changed,objcopy_ihex)

# same for bin. bin is not build but you can add it if you whish
$(APP_BUILD_DIR)/$(BIN_NAME): $(APP_BUILD_DIR)/$(ELF_NAME)
	$(call if_changed,objcopy_bin)

# special target to create the application build directory
$(APP_BUILD_DIR):
	$(call cmd,mkdir)


-include $(DEP)
