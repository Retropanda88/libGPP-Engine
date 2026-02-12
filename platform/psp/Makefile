# === libGPP-Engine — psp ===
PSPDEV ?= /usr/local/PSPDev
PSPSDK ?= $(PSPDEV)/psp/sdk

PSP_CC   ?= psp-gcc
PSP_CXX  ?= psp-g++
PSP_AR   ?= psp-ar
MKSFO    ?= mksfo
PACK_PBP ?= pack-pbp
FIXUP    ?= psp-fixup-imports
STRIP    ?= psp-strip
RM       ?= rm -f

# Flags
CFLAGS   := -O3 -G0 -Wall -DPSP_BUILD -I$(PSPSDK)/include -I../../include -I../../src
CFLAGS +=
CXXFLAGS := $(CFLAGS) -fno-exceptions -fno-rtti
LDFLAGS  := -L$(PSPSDK)/lib

# Paths
SRCDIR  := ../../src
TESTDIR := ../../tests/test_engine
OBJDIR  := obj
BINDIR  := bin

TARGET_BASENAME := test_psp
TARGET_ELF      := $(BINDIR)/$(TARGET_BASENAME).elf
SFO_FILE        := $(BINDIR)/PARAM.SFO
EBOOT_FILE      := $(BINDIR)/EBOOT.PBP

# ensure dirs
$(shell mkdir -p $(OBJDIR) $(BINDIR))

# collect sources (find, with simple fallback for MSYS1)
SRC := $(shell find $(SRCDIR) -type f \( -name "*.c" -o -name "*.cpp" \) 2>/dev/null)
ifeq ($(strip $(SRC)),)
  SUBDIRS := $(shell ls -1 $(SRCDIR) 2>/dev/null)
  SRC := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*.cpp)
  SRC += $(foreach d,$(SUBDIRS),$(wildcard $(SRCDIR)/$(d)/*.c) $(wildcard $(SRCDIR)/$(d)/*.cpp))
endif

TEST_MAIN := $(TESTDIR)/main.cpp

# map sources -> objects (keeps subdir structure under obj/)
OBJS := $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SRC:.cpp=.o))
OBJS := $(OBJS:.c=.o)
OBJS += $(OBJDIR)/test_engine.o

# Libraries (order is important)
LIBS := \
    -lSDL_image -lSDL \
    -lpng -ljpeg -lz \
    -lpspaudio -lpspirkeyb -lpsppower -lpsphprm \
    -lpspgu -lpspctrl -lpspge \
    -lpspdebug -lpspdisplay \
    -lpspsdk -lstdc++\
    -lc -lm \
    -lpspuser\
    -lpspkernel

# Rules
.PHONY: all elf eboot clean print-vars

all: eboot

elf: $(TARGET_ELF)

# compile C++
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(PSP_CXX) $(CXXFLAGS) -c $< -o $@

# compile C
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(PSP_CC) $(CFLAGS) -c $< -o $@

# test main
$(OBJDIR)/test_engine.o: $(TEST_MAIN)
	@mkdir -p $(dir $@)
	$(PSP_CXX) $(CXXFLAGS) -c $< -o $@

# link with psp-gcc (IMPORTANT)
$(TARGET_ELF): $(OBJS)
	@echo "Linking ELF -> $@"
	$(PSP_CC) $(LDFLAGS) -o $@ $(filter %,$(OBJS)) $(LIBS)
	-$(FIXUP) $@

# PARAM.SFO
PSP_EBOOT_TITLE ?= "test engine"
$(SFO_FILE):
	@echo "Creating PARAM.SFO -> $@"
	$(MKSFO) $(PSP_EBOOT_TITLE) $@

$(EBOOT_FILE): $(TARGET_ELF) $(SFO_FILE)
	@echo "Packing EBOOT.PBP -> $@"
	-$(STRIP) $(TARGET_ELF) -o $(TARGET_ELF)_strip.elf || true
	$(PACK_PBP) $@ $(SFO_FILE) \
        NULL \
        NULL \
        NULL \
        NULL \
        NULL \
        $(TARGET_ELF)_strip.elf \
        NULL
	-$(RM) $(TARGET_ELF)_strip.elf


eboot: $(EBOOT_FILE)

clean:
	-$(RM) -rf $(OBJDIR) $(BINDIR) PARAM.SFO EBOOT.PBP $(TARGET_ELF)

print-vars:
	@echo "PSPSDK = $(PSPSDK)"
	@echo "SRCDIR = $(SRCDIR)"
	@echo "Found SRC files:"; echo $(SRC)
	@echo "OBJ files to build:"; echo $(OBJS)
