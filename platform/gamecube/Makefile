# ===========================================================================
#         Makefile – GameCube Backend for libGPP-Engine (TEST MODE)
# ===========================================================================

ROOT_DIR      := ../../
SRC_DIR       := $(ROOT_DIR)src/
INC_DIR       := $(ROOT_DIR)include/
TEST_DIR      := $(ROOT_DIR)tests/test_engine/
NES_LIB_DIR   := $(ROOT_DIR)nes/lib/gc/

BIN_DIR       := bin
OBJ_DIR       := obj

TARGET        := $(BIN_DIR)/test_gc.elf
TARGET_DOL    := $(BIN_DIR)/test_gc.dol

# Crear carpetas
$(shell mkdir -p $(BIN_DIR))
$(shell mkdir -p $(OBJ_DIR))

# ===========================================================================
#   Toolchain GameCube (devkitPPC)
# ===========================================================================
DEVKITPRO  := /usr/local/gcwiidev
DEVKITPPC  := $(DEVKITPRO)/devkitPPC

include $(DEVKITPPC)/gamecube_rules

export DEVKITPRO DEVKITPPC
export PATH := $(DEVKITPPC)/bin:$(PATH)

CC  := $(DEVKITPPC)/bin/powerpc-eabi-gcc
CXX := $(DEVKITPPC)/bin/powerpc-eabi-g++
LD  := $(DEVKITPPC)/bin/powerpc-eabi-gcc

# ===========================================================================
#   Flags para compilación
# ===========================================================================
# Añadimos TEST_DIR y la carpeta base nes para resolver includes como "nes.h" y de pruebas
GC_INCS = -I$(INC_DIR) -I$(SRC_DIR) -I$(ROOT_DIR)nes/src -I$(ROOT_DIR)nes -I$(TEST_DIR) -I$(DEVKITPRO)/libogc/include -I$(DEVKITPRO)/portlibs/ppc/include

CFLAGS   = -O2 -mcpu=750 -meabi -mhard-float -Wall -DGEKKO -D__cube__ -DGC_BUILD $(GC_INCS)
CXXFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions

# Quitamos -L$(NES_LIB_DIR) porque vamos a inyectar la librería directo por su ruta física
GC_LDFLAGS = -L$(DEVKITPRO)/libogc/lib/cube -L$(DEVKITPRO)/portlibs/ppc/lib -Wl,-Map,$(BIN_DIR)/test_gc.map

# Removemos -lnes de aquí porque se acopla directo abajo
GC_LIBS    = -lSDL_image -lSDL_mixer -lSDL -ljpeg -lpng -ltiff -lmikmod -lsmpeg -lmad -laesnd -lstdc++ -lfat -logc -lm -lz

# ===========================================================================
#   Fuentes y objetos automáticos (Copiado de la lógica funcional de PS2)
# ===========================================================================
# Fuentes del motor
CPP_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
C_SOURCES   := $(shell find $(SRC_DIR) -type f -name "*.c")

# Fuentes de la carpeta de pruebas (¡NUEVO AUTOMÁTICO!)
TEST_CPP_SRC := $(shell find $(TEST_DIR) -type f -name "*.cpp")
TEST_C_SRC   := $(shell find $(TEST_DIR) -type f -name "*.c")

# Objetos mapeados a sus respectivas subcarpetas dentro de /obj
CPP_OBJS     := $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)/src/%.o,$(CPP_SOURCES))
C_OBJS       := $(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)/src/%.o,$(C_SOURCES))
TEST_CPP_OBJ := $(patsubst $(TEST_DIR)%.cpp,$(OBJ_DIR)/tests/%.o,$(TEST_CPP_SRC))
TEST_C_OBJ   := $(patsubst $(TEST_DIR)%.c,$(OBJ_DIR)/tests/%.o,$(TEST_C_SRC))

# Todos los objetos juntos
OBJS := $(CPP_OBJS) $(C_OBJS) $(TEST_CPP_OBJ) $(TEST_C_OBJ)

# ===========================================================================
#   Reglas principales
# ===========================================================================
all: $(TARGET_DOL)

# Compilar .cpp del motor
$(OBJ_DIR)/src/%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Compilar .c del motor
$(OBJ_DIR)/src/%.o: $(SRC_DIR)%.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

# Compilar .cpp de pruebas (¡NUEVO!)
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Compilar .c de pruebas (¡NUEVO!)
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)%.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

# Link ELF (Inyectamos libnes.a al inicio de las librerías con ruta explícita)
$(TARGET): $(OBJS)
	$(LD) $(MACHDEP) $(GC_LDFLAGS) $(OBJS) $(NES_LIB_DIR)libnes.a $(GC_LIBS) -o $@

# Convertir a DOL
$(TARGET_DOL): $(TARGET)
	elf2dol $< $@

# ===========================================================================
#   Limpieza
# ===========================================================================
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) *.elf *.dol

rebuild: clean all