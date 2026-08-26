# ===========================================================================
#           Makefile – PSP Backend for libGPP-Engine (TEST MODE)
# ===========================================================================

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

# Paths (Aseguramos la barra diagonal / al final)
ROOT_DIR    := ../../
SRCDIR      := $(ROOT_DIR)src
INC_DIR     := $(ROOT_DIR)include
TESTDIR     := $(ROOT_DIR)tests/test_engine
NES_LIB_DIR := $(ROOT_DIR)nes/lib/psp/

OBJDIR      := obj
BINDIR      := bin

# Flags (Inyectamos la cabecera de la subcarpeta nes/src y el TESTDIR para los .h de pruebas)
CFLAGS   := -O3 -G0 -Wall -DPSP_BUILD -I$(PSPSDK)/include -I$(INC_DIR) -I$(SRCDIR) -I$(ROOT_DIR)nes/src -I$(TESTDIR)
CXXFLAGS := $(CFLAGS) -fno-exceptions -fno-rtti

# LDFLAGS: Mantenemos la ruta de librerías del SDK
LDFLAGS  := -L$(PSPSDK)/lib

TARGET_BASENAME := test_psp
TARGET_ELF      := $(BINDIR)/$(TARGET_BASENAME).elf
SFO_FILE        := $(BINDIR)/PARAM.SFO
EBOOT_FILE      := $(BINDIR)/EBOOT.PBP

# Ensure dirs
$(shell mkdir -p $(OBJDIR) $(BINDIR))

# ---------------------------------------------------------------------------
#  1. RECOLECTAR FUENTES (MOTOR + PRUEBAS AUTOMÁTICAS)
# ---------------------------------------------------------------------------

# Fuentes del MOTOR
SRC := $(shell find $(SRCDIR) -type f \( -name "*.c" -o -name "*.cpp" \) 2>/dev/null)
ifeq ($(strip $(SRC)),)
  SUBDIRS := $(shell ls -1 $(SRCDIR) 2>/dev/null)
  SRC := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*.cpp)
  SRC += $(foreach d,$(SUBDIRS),$(wildcard $(SRCDIR)/$(d)/*.c) $(wildcard $(SRCDIR)/$(d)/*.cpp))
endif

# Fuentes de las PRUEBAS
TEST_SRC := $(shell find $(TESTDIR) -type f \( -name "*.c" -o -name "*.cpp" \) 2>/dev/null)
ifeq ($(strip $(TEST_SRC)),)
  TEST_SUBDIRS := $(shell ls -1 $(TESTDIR) 2>/dev/null)
  TEST_SRC := $(wildcard $(TESTDIR)/*.c) $(wildcard $(TESTDIR)/*.cpp)
  TEST_SRC += $(foreach d,$(TEST_SUBDIRS),$(wildcard $(TESTDIR)/$(d)/*.c) $(wildcard $(TESTDIR)/$(d)/*.cpp))
endif

# ---------------------------------------------------------------------------
#  2. MAPEAR FUENTES A OBJETOS
# ---------------------------------------------------------------------------

# Objetos del MOTOR (van a obj/src/...)
OBJS_ENGINE := $(patsubst $(SRCDIR)/%, $(OBJDIR)/src/%, $(SRC:.cpp=.o))
OBJS_ENGINE := $(OBJS_ENGINE:.c=.o)

# Objetos de las PRUEBAS (van a obj/tests/...)
OBJS_TEST := $(patsubst $(TESTDIR)/%, $(OBJDIR)/tests/%, $(TEST_SRC:.cpp=.o))
OBJS_TEST := $(OBJS_TEST:.c=.o)

# Juntamos todos los objetos para el Linker
OBJS := $(OBJS_ENGINE) $(OBJS_TEST)

# Libraries (Inyectamos la ruta directa del .a para evitar fallos de rutas mixtas en Windows)
LIBS := \
    $(NES_LIB_DIR)libnes.a \
    -lSDL_image -lSDL \
    -lpng -ljpeg -lz \
    -lpspaudio -lpspirkeyb -lpsppower -lpsphprm \
    -lpspgu -lpspctrl -lpspge \
    -lpspdebug -lpspdisplay \
    -lpspsdk -lstdc++ \
    -lc -lm \
    -lpspuser \
    -lpspkernel

# Rules
.PHONY: all elf eboot clean print-vars

all: eboot

elf: $(TARGET_ELF)

# ---------------------------------------------------------------------------
#  3. REGLAS DE COMPILACIÓN
# ---------------------------------------------------------------------------

# Compilar C++ del MOTOR
$(OBJDIR)/src/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(PSP_CXX) $(CXXFLAGS) -c $< -o $@

# Compilar C del MOTOR
$(OBJDIR)/src/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(PSP_CC) $(CFLAGS) -c $< -o $@

# Compilar C++ de las PRUEBAS
$(OBJDIR)/tests/%.o: $(TESTDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(PSP_CXX) $(CXXFLAGS) -c $< -o $@

# Compilar C de las PRUEBAS
$(OBJDIR)/tests/%.o: $(TESTDIR)/%.c
	@mkdir -p $(dir $@)
	$(PSP_CC) $(CFLAGS) -c $< -o $@

# Enlazar con psp-gcc
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
	@echo "TESTDIR = $(TESTDIR)"
	@echo "NES_LIB_DIR = $(NES_LIB_DIR)"
	@echo "Found MOTOR SRC files:"; echo $(SRC)
	@echo "Found TEST SRC files:"; echo $(TEST_SRC)
	@echo "All OBJ files to build:"; echo $(OBJS)