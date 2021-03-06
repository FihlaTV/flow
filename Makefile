################################################################################
# Makefile for the Dynamics Toolset
# Copyright (c) 2006-2008 Jordan Van Aalsburg
#
# This file is part of the Dynamics Toolset.
#
# The Dynamics Toolset is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version3 3 of the License, or (at your option) any 
# later version.
# 
# The Dynamics Toolset is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
# details.
#
# You should have received a copy of the GNU General Public License
# along with the Dynamics Toolset. If not, see <http://www.gnu.org/licenses/>.
################################################################################

# Set the Vrui installation directory
#
VRUI_MAKEDIR = /usr/share/Vrui

# Set the installation root directory
#
INSTALLDIR = /usr/local/flow

ifeq ($(INSTALLDIR), /)
	BININSTALLDIR = /usr/bin
	ETCINSTALLDIR = /etc/flow
	SHAREINSTALLDIR = /usr/share/flow
else
	# Assumption: "flow" appears in INSTALLDIR
	BININSTALLDIR = $(INSTALLDIR)/bin
	ETCINSTALLDIR = $(INSTALLDIR)/etc
	SHAREINSTALLDIR = $(INSTALLDIR)/share

endif

# Object and dependency file locations
#
BUILD_DIR = build
OBJECT_DIR = $(BUILD_DIR)/obj
DEPEND_DIR = $(BUILD_DIR)/dep
LIB_DIR = $(BUILD_DIR)/lib
PLUGIN_DIR = plugins


# Program name
#
PROGRAM=flow
VERSION=1.0

ifeq ($(shell uname -s),Darwin)
  SYSTEM_NAME = Darwin
endif
ifeq ($(shell uname -s),Linux)
  SYSTEM_NAME = Linux
endif
ifndef SYSTEM_NAME
  $(error Unsupported system type)
endif

# Compiler options
#
CC     = g++
CFLAGS = -Wall
OPT    = -mfpmath=sse -msse2 -mmmx -g0 -DNDEBUG -O3

## If using MESA and getting GL enum errors, set this flag
#OPT    += -DMESA 

## If you have a ghetto video card that claims to support shaders
## but actually displays nothing, set this flag
#OPT     += -DGHETTO

OPT += -ggdb

# ftgl font renderer
BASEDIR = /usr
LOCAL_LINK += -lftgl -lfreetype

LOCAL_INCLUDE += -Isrc -Isrc/Dynamics -Isrc/External -Isrc/External/VruiSupport -Isrc/ToolBox -I$(BASEDIR)/include/freetype2
LOCAL_LINK += -lGLU -lgle

# Need to fix makedepend.  Changes in toolbox header files do not notify flow
TOOLBOX = $(LIB_DIR)/libToolBox.a
TOOLBOX_SOURCES = $(shell find src/ToolBox -name "*.cpp")

# Search plugin directory and generate list of plugin names
#
# For now, we only support Experiment plugins. This should be changed to support
# other types of plugins: Transformers, DynamicalModels, Integrators, etc.
#
PLUGINS_OBJECTS = $(addprefix $(OBJECT_DIR),$(subst .cpp,.o,$(subst ./src,,$(shell find ./src/Experiments -name *Experiment.cpp))))
PLUGINS = $(addprefix $(PLUGIN_DIR)/lib,$(subst .cpp,.so,$(subst ./src/Experiments/,,$(shell find ./src/Experiments -name *Experiment.cpp))))

# Project source files
#
SOURCES = 											\
	src/FieldViewer.cpp								\
	src/main.cpp									\
	src/External/VruiSupport/VruiStreamManip.cpp	\
	src/Tools/AbstractDynamicsTool.cpp              \
	src/Tools/DotSpreaderTool.cpp                  \
	src/Tools/DotSpreaderOptionsDialog.cpp   		\
	src/Tools/DynamicSolverTool.cpp                  \
	src/Tools/DynamicSolverOptionsDialog.cpp   		\
	src/Tools/ParticleSprayerTool.cpp                  \
	src/Tools/ParticleSprayerOptionsDialog.cpp   		\
	src/Tools/StaticSolverTool.cpp                  \
	src/Tools/StaticSolverOptionsDialog.cpp   		\
	src/DataItem.cpp								\
	src/External/VruiSupport/VruiStreamManip.cpp        \
	src/FrameRateDialog.cpp                             \
	src/PositionDialog.cpp                              \
	src/ExperimentDialog.cpp                            \
	src/FieldViewer_ui.cpp                         
	

include $(VRUI_MAKEDIR)/Vrui.makeinclude

ifndef VERBOSE 
	QUIET = @
endif


## For plugins, we create the .o files from .so targets and the .o
## files did not exist beforehand (in general).  Unless the .d files exist
## (thus creating an explicit target for the .o files), then the .o
## files are deemed "intermediate" and make will delete them when
## finished.  Since we are now creating the .d files, this should only
## be an issue the first compile time.  Or we make make this issue
## go away altogether, by listing the .o files as explicit targets.
## Then they are built before the .so files and are considered by the
## rule for creating the .so files NOT to be intermediate anymore.
## This is done below.

.PHONY: all
all: $(TOOLBOX) $(PROGRAM) $(PLUGINS_OBJECTS) $(PLUGINS)

.PHONY: test
test: 
	$(QUIET)$(CC) -c -ggdb -o main.o $(CFLAGS) $(LOCAL_INCLUDE) $(VRUI_CFLAGS) $(OPT) src/Dynamics/main.cpp
	$(QUIET)$(CC) $(CFLAGS) -o test main.o
	
# Main program
#
$(PROGRAM): $(SOURCES:src/%.cpp=$(OBJECT_DIR)/%.o)
	@echo Linking executable $@...
	$(QUIET)$(CC) $(CFLAGS) -rdynamic -o $@ $^ $(LOCAL_LINK) $(TOOLBOX) $(VRUI_LINKFLAGS) -ldl


# ToolBox library (static)
#
$(TOOLBOX): $(TOOLBOX_SOURCES:src/%.cpp=$(OBJECT_DIR)/%.o)
	@echo Creating  $@...
	$(QUIET)mkdir -p $(LIB_DIR)
	$(QUIET)$(AR) rcs $@ $^


#
# Generic build rules
#

ifneq "$(MAKECMDGOALS)" "clean"
 -include $(SOURCES:src/%.cpp=./$(DEPEND_DIR)/%.d)
 -include $(TOOLBOX_SOURCES:src/%.cpp=$(DEPEND_DIR)/%.d)
 -include $(PLUGINS:$(PLUGIN_DIR)/lib%.so=$(DEPEND_DIR)/Experiments/%.d)
endif

# $(call make-depend,source-file,object-file,depend-file)
define make-depend
  $(CC) -MM -MF $3 -MP -MT $2 $(CFLAGS) $(LOCAL_INCLUDE) $(VRUI_CFLAGS) $1
endef


# Plugin object files
#
$(OBJECT_DIR)/Experiments/%.o: src/Experiments/%.cpp
	$(QUIET)mkdir -p $(OBJECT_DIR)/Experiments
	$(QUIET)mkdir -p $(DEPEND_DIR)/Experiments
	@echo [plugin] Compiling $<...
	$(QUIET)$(call make-depend,$<,$@,$(@:$(OBJECT_DIR)/%.o=$(DEPEND_DIR)/%.d))
	$(QUIET)$(CC) $(CFLAGS) $(LOCAL_INCLUDE) $(VRUI_CFLAGS) -fPIC -c -g -o $@ $<

# Regular object files
#
$(OBJECT_DIR)/%.o: src/%.cpp
	$(QUIET)mkdir -p $(OBJECT_DIR)
	@echo Compiling $<...
	$(QUIET)mkdir -p $(OBJECT_DIR)/$(*D)
	$(QUIET)mkdir -p $(DEPEND_DIR)/$(*D)
	$(QUIET)$(call make-depend,$<,$@,$(@:$(OBJECT_DIR)/%.o=$(DEPEND_DIR)/%.d))
	$(QUIET)$(CC) -c -g -o $@ $(CFLAGS) $(LOCAL_INCLUDE) $(VRUI_CFLAGS) $(OPT) $<

$(OBJECT_DIR)/FieldViewer.o: CFLAGS += -DRESOURCEDIR='"$(SHAREINSTALLDIR)"'

ifeq "$(SYSTEM_NAME)" "Darwin"
define plugin-compile
	$(CC) -dynamiclib -single_module -install_name $1 -undefined dynamic_lookup -o $2 $3
endef
else
define plugin-compile
	$(CC) -shared -Wl,-soname,$1 -o $2 $3
endef
endif

# Dynamic libraries (plugins)
#
$(PLUGIN_DIR)/lib%.so: $(OBJECT_DIR)/Experiments/%.o
	$(QUIET)mkdir -p $(PLUGIN_DIR)
	@echo "[plugin] Creating $@..."
	$(QUIET)$(call plugin-compile,$(subst $(PLUGIN_DIR)/,,$@),$@,$^)
	$(QUIET)ln -sf $(subst $(PLUGIN_DIR)/,,$@) $@.1

# Unset all default rules
#
%: ;

#
# Utility targets
#

.PHONY: clean
clean:
	@echo "Removing object files and dependencies..."
	$(QUIET)rm -rf $(BUILD_DIR)
	@echo "Removing libraries and binaries..."
	$(QUIET)rm -rf $(PLUGIN_DIR) $(PROGRAM)


BACKUP_FILES = $(subst ./,,$(shell find . -name "*~"))

.PHONY: squeaky
squeaky: clean
	@echo "Removing plugins..."	
	$(QUIET)rm -rf $(PLUGIN_DIR)
	@echo "Removing backup files..."
	$(QUIET)rm -f $(BACKUP_FILES)	

.PHONY: distclean
distclean: squeaky
	@echo "Removing documentation..."
	$(QUIET)rm -rf doc


.PHONY: install
install: all
	@echo "Installing program and libraries..."
	$(QUIET)mkdir -p $(BININSTALLDIR) 
	$(QUIET)mkdir -p $(ETCINSTALLDIR)
	$(QUIET)mkdir -p $(SHAREINSTALLDIR)
	$(QUIET)mkdir -p $(SHAREINSTALLDIR)/plugins
	$(QUIET)cp $(PROGRAM) $(BININSTALLDIR)/
	$(QUIET)cp -r etc/*   $(ETCINSTALLDIR)/
	$(QUIET)cp -r images  $(SHAREINSTALLDIR)/
	$(QUIET)cp -r $(PLUGIN_DIR)/* $(SHAREINSTALLDIR)/plugins/
	$(QUIET)cp -r fonts   $(SHAREINSTALLDIR)/
	$(QUIET)cp -r views   $(SHAREINSTALLDIR)/

# Code documentation
#
ifdef VERBOSE
  MAKEDOC = doxygen
else
  MAKEDOC = doxygen > /dev/null
endif

.PHONY: doc
doc:
	@echo "generating documentation"
	@$(MAKEDOC)

.PHONY: debug
debug:
	@echo "PLUGINS:"
	@echo
	@echo $(PLUGINS)
