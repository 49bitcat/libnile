# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Adrian "asie" Siekierka, 2023

WONDERFUL_TOOLCHAIN ?= /opt/wonderful
TARGET ?= wswan/medium
include $(WONDERFUL_TOOLCHAIN)/target/$(TARGET)/makedefs.mk

# Source code paths
# -----------------

INCLUDEDIRS	:= core fatfs/source include storage
CORE_SRCDIRS	:= core
STORAGE_SRCDIRS	:= fatfs/source storage

# Defines passed to all files
# ---------------------------

DEFINES		:=

# Libraries
# ---------

LIBDIRS		:= $(WF_ARCH_LIBDIRS)

# Build artifacts
# ---------------

BUILDDIR	:= build/$(TARGET)
CORE_ARCHIVE	:= lib/libnile.a
STORAGE_ARCHIVE	:= lib/libnilefs.a

# Verbose flag
# ------------

ifeq ($(V),1)
_V		:=
else
_V		:= @
endif

# Source files
# ------------

SOURCES_CORE_S		:= $(shell find -L $(CORE_SRCDIRS) -name "*.s")
SOURCES_CORE_C		:= $(shell find -L $(CORE_SRCDIRS) -name "*.c")
SOURCES_STORAGE_S	:= $(shell find -L $(STORAGE_SRCDIRS) -name "*.s")
SOURCES_STORAGE_C	:= $(shell find -L $(STORAGE_SRCDIRS) -name "*.c")

# Compiler and linker flags
# -------------------------

WARNFLAGS	:= -Wall

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path)) \
		   $(foreach path,$(LIBDIRS),-isystem $(path)/include)

ASFLAGS		+= -x assembler-with-cpp $(DEFINES) $(WF_ARCH_CFLAGS) \
		   $(INCLUDEFLAGS) -ffunction-sections -fdata-sections

CFLAGS		+= -std=gnu11 $(WARNFLAGS) $(DEFINES) $(WF_ARCH_CFLAGS) \
		   $(INCLUDEFLAGS) -ffunction-sections -fdata-sections -O2

# Intermediate build files
# ------------------------

CORE_OBJS	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CORE_S))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CORE_C)))

STORAGE_OBJS	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_STORAGE_S))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_STORAGE_C)))

OBJS		:= $(CORE_OBJS) $(STORAGE_OBJS)

DEPS		:= $(OBJS:.o=.d)

# Targets
# -------

.PHONY: all clean doc

all: $(CORE_ARCHIVE) $(STORAGE_ARCHIVE) compile_commands.json

$(CORE_ARCHIVE): $(CORE_OBJS)
	@echo "  AR      $@"
	@$(MKDIR) -p $(@D)
	$(_V)$(AR) rcs $@ $(CORE_OBJS)

$(STORAGE_ARCHIVE): $(STORAGE_OBJS)
	@echo "  AR      $@"
	@$(MKDIR) -p $(@D)
	$(_V)$(AR) rcs $@ $(STORAGE_OBJS)

clean:
	@echo "  CLEAN"
	$(_V)$(RM) $(BUILDDIR) compile_commands.json lib

compile_commands.json: $(OBJS) | Makefile
	@echo "  MERGE   compile_commands.json"
	$(_V)$(WF)/bin/wf-compile-commands-merge $@ $(patsubst %.o,%.cc.json,$^)

doc:
	@echo "  MKDIR    build/doc"
	$(_V)$(MKDIR) -p build/doc
	@echo "  DOXYGEN"
	doxygen

# Rules
# -----

$(BUILDDIR)/%.s.o : %.s
	@echo "  AS      $<"
	@$(MKDIR) -p $(@D)
	$(_V)$(CC) $(ASFLAGS) -MMD -MP -MJ $(patsubst %.o,%.cc.json,$@) -c -o $@ $<

$(BUILDDIR)/%.c.o : %.c
	@echo "  CC      $<"
	@$(MKDIR) -p $(@D)
	$(_V)$(CC) $(CFLAGS) -MMD -MP -MJ $(patsubst %.o,%.cc.json,$@) -c -o $@ $<

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
