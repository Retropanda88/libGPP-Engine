#===========================================================
#      Makefile – GameCube Backend for libGPP-Engine
# ===========================================================

ROOT_DIR      := ../../
SRC_DIR       := $(ROOT_DIR)src/
INC_DIR       := $(ROOT_DIR)include/
TEST_DIR      := $(ROOT_DIR)tests/test_engine/

BIN_DIR       := bin
OBJ_DIR       := obj

TARGET        := $(BIN_DIR)/test_gc.elf
TARGET_DOL    := $(BIN_DIR)/test_gc.dol

# Crear carpetas
$(shell mkdir -p $(BIN_DIR))
$(shell mkdir -p $(OBJ_DIR))

# ===========================================================
#   Toolchain GameCube (devkitPPC)
# ===========================================================
DEVKITPRO  := /usr/local/gcwiidev
DEVKITPPC  := $(DEVKITPRO)/devkitPPC

include $(DEVKITPPC)/gamecube_rules

export DEVKITPRO DEVKITPPC
export PATH := $(DEVKITPPC)/bin:$(PATH)

CC  := $(DEVKITPPC)/bin/powerpc-eabi-gcc
CXX := $(DEVKITPPC)/bin/powerpc-eabi-g++
LD  := $(DEVKITPPC)/bin/powerpc-eabi-gcc

# ===========================================================
#  Flags para compilación
# ===========================================================
GC_INCS = -I$(INC_DIR) -I$(SRC_DIR) -I$(DEVKITPRO)/libogc/include -I$(DEVKITPRO)/portlibs/ppc/include

CFLAGS   = -O2 -mcpu=750 -meabi -mhard-float -Wall -DGEKKO -D__cube__ -DGC_BUILD $(GC_INCS)
CFLAGS += 
CXXFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions

GC_LDFLAGS = -L$(DEVKITPRO)/libogc/lib/cube -L$(DEVKITPRO)/portlibs/ppc/lib -Wl,-Map,$(BIN_DIR)/test_gc.map
GC_LIBS    = -lSDL_image -lSDL_mixer -lSDL -ljpeg -lpng -ltiff -lmikmod -lsmpeg -lmad -laesnd -lstdc++ -lfat -logc -lm -lz

# ===========================================================
#  Fuentes y objetos
# ===========================================================
CPP_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
C_SOURCES   := $(shell find $(SRC_DIR) -type f -name "*.c")
TEST_SOURCE := $(TEST_DIR)/main.cpp

CPP_OBJS := $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)/%.o,$(CPP_SOURCES))
C_OBJS   := $(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
TEST_OBJ := $(OBJ_DIR)/test_engine.o

OBJS := $(CPP_OBJS) $(C_OBJS) $(TEST_OBJ)

# ===========================================================
#  Reglas principales
# ===========================================================
all: $(TARGET_DOL)

# Compilar .cpp del motor
$(OBJ_DIR)/%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Compilar .c del motor
$(OBJ_DIR)/%.o: $(SRC_DIR)%.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

# Compilar test
$(TEST_OBJ): $(TEST_SOURCE)
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Link ELF
#$(TARGET): $(OBJS)
#	$(LD) $(OBJS) $(GC_LDFLAGS) $(GC_LIBS) -o $@

$(TARGET): $(OBJS)
	$(LD) $(MACHDEP) $(GC_LDFLAGS) $(OBJS) $(GC_LIBS) -o $@



# Convertir a DOL
$(TARGET_DOL): $(TARGET)
	elf2dol $< $@

# ===========================================================
#  Limpieza
# ===========================================================
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) *.elf *.dol

rebuild: clean all
