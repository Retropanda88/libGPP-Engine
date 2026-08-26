#===========================================================
#        Makefile – PS2 Backend for libGPP-Engine (TEST MODE)
#===========================================================

ROOT_DIR      = ../../
SRC_DIR       = $(ROOT_DIR)src/
INC_DIR       = $(ROOT_DIR)include/
TEST_DIR      = $(ROOT_DIR)tests/test_engine/
NES_LIB_DIR   = $(ROOT_DIR)nes/lib/ps2/

BIN_DIR       = bin/
OBJ_DIR       = obj/

TARGET        = $(BIN_DIR)test_ps2.elf

# Crear carpetas base
$(shell mkdir -p $(BIN_DIR))
$(shell mkdir -p $(OBJ_DIR))

# -----------------------------------------------------------
#        RECOLECTAR FUENTES DEL MOTOR + TESTS AUTOMÁTICOS
# -----------------------------------------------------------

# Fuentes del motor
EE_CPP_SRC    = $(shell find $(SRC_DIR) -type f -name "*.cpp")
EE_C_SRC      = $(shell find $(SRC_DIR) -type f -name "*.c")

# Fuentes de la carpeta de pruebas (¡NUEVO!)
TEST_CPP_SRC  = $(shell find $(TEST_DIR) -type f -name "*.cpp")
TEST_C_SRC    = $(shell find $(TEST_DIR) -type f -name "*.c")

# Objetos del motor
EE_CPP_OBJS   = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)src/%.o,$(EE_CPP_SRC))
EE_C_OBJS     = $(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)src/%.o,$(EE_C_SRC))

# Objetos de las pruebas (¡NUEVO!)
TEST_CPP_OBJS = $(patsubst $(TEST_DIR)%.cpp,$(OBJ_DIR)tests/%.o,$(TEST_CPP_SRC))
TEST_C_OBJS   = $(patsubst $(TEST_DIR)%.c,$(OBJ_DIR)tests/%.o,$(TEST_C_SRC))

# Todos los objetos juntos
EE_OBJS       = $(EE_CPP_OBJS) $(EE_C_OBJS) $(TEST_CPP_OBJS) $(TEST_C_OBJS)

# -----------------------------------------------------------
#                      INCLUDES DEL MOTOR
# -----------------------------------------------------------

# Añadimos TEST_DIR para que encuentre los archivos .h de pruebas automáticamente
EE_INCS = -I$(INC_DIR) -I$(SRC_DIR) -I$(ROOT_DIR)nes/src -I$(TEST_DIR)
EE_INCS += -I$(PS2SDK)/ports/include
EE_INCS += -I$(PS2DEV)/gsKit/ee/gs/include -I$(PS2DEV)/gsKit/ee/dma/include
# Asegúrate de que incluya la carpeta base "nes" además de "nes/src"
EE_INCS += -I../../nes -I../../nes/src
EE_INCS += -I.

EE_CFLAGS   = -O2 -G0 -Wall -DPS2_BUILD -DPS2_DEBUG $(EE_INCS) 
EE_CXXFLAGS = $(EE_CFLAGS) -fno-exceptions -fno-rtti

# -----------------------------------------------------------
#                     LIBRERÍAS PS2 SDK
# -----------------------------------------------------------

EE_LIBS = \
	-L$(PS2DEV)/gsKit/lib \
	-lgskit -ldmakit \
	-L$(NES_LIB_DIR) \
	-L$(PS2SDK)/ports/lib \
	-lnes -lSDL_ttf -lsdlmixer -lSDL_image -lfreetype -lpng -ljpeg -lSDL \
	-laudsrv -lcdvd \
	-L$(PS2SDK)/ee/lib \
	-lm -lc -lkernel -lstdc++

# -----------------------------------------------------------
#      Incluir toolchain SIN generar reglas automáticas
# -----------------------------------------------------------

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp

# -----------------------------------------------------------
#                  REGLAS DE COMPILACIÓN
# -----------------------------------------------------------

all: $(TARGET)

# Compilar .cpp del motor
$(OBJ_DIR)src/%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(EE_CXX) -c $< -o $@ $(EE_CXXFLAGS)

# Compilar .c del motor
$(OBJ_DIR)src/%.o: $(SRC_DIR)%.c
	@mkdir -p $(dir $@)
	$(EE_CC) -c $< -o $@ $(EE_CFLAGS)

# Compilar .cpp de pruebas (¡NUEVO!)
$(OBJ_DIR)tests/%.o: $(TEST_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(EE_CXX) -c $< -o $@ $(EE_CXXFLAGS)

# Compilar .c de pruebas (¡NUEVO!)
$(OBJ_DIR)tests/%.o: $(TEST_DIR)%.c
	@mkdir -p $(dir $@)
	$(EE_CC) -c $< -o $@ $(EE_CFLAGS)

# Link final del ELF
$(TARGET): $(EE_OBJS)
	$(EE_CC) -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(EE_CXXFLAGS) \
		-o $(TARGET) $(PS2SDK)/ee/startup/crt0.o $(EE_OBJS) $(EE_LDFLAGS) $(EE_LIBS) -lcdvd -lkernel

# -------------------------
# LIMPIEZA REAL
# -------------------------
clean:
	#rm -rf $(OBJ_DIR) $(BIN_DIR)

# -------------------------
# REBUILD
# -------------------------
.PHONY: rebuild clean all run reset emu
rebuild: clean all

run: $(TARGET)
	ps2client execee host:$(TARGET)

reset:
	ps2client reset

emu: $(TARGET)
	pcsx2 --elf $(TARGET)