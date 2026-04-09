#===========================================================
#      Makefile – PS2 Backend for libGPP-Engine (TEST MODE)
#===========================================================

ROOT_DIR      = ../../
SRC_DIR       = $(ROOT_DIR)src/
INC_DIR       = $(ROOT_DIR)include/
TEST_DIR      = $(ROOT_DIR)tests/test_engine/

BIN_DIR       = bin/
OBJ_DIR       = obj/

TARGET        = $(BIN_DIR)test_ps2.elf

# Crear carpetas
$(shell mkdir -p $(BIN_DIR))
$(shell mkdir -p $(OBJ_DIR))

# -----------------------------------------------------------
#              RECOLECTAR FUENTES DEL MOTOR + TEST
# -----------------------------------------------------------

EE_CPP_SRC    = $(shell find $(SRC_DIR) -type f -name "*.cpp")
EE_C_SRC      = $(shell find $(SRC_DIR) -type f -name "*.c")
TEST_MAIN_CPP = $(TEST_DIR)main.cpp

EE_CPP_OBJS   = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(EE_CPP_SRC))
EE_C_OBJS     = $(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)%.o,$(EE_C_SRC))
TEST_OBJS     = $(OBJ_DIR)test_engine.o

EE_OBJS       = $(EE_CPP_OBJS) $(EE_C_OBJS) $(TEST_OBJS)

# -----------------------------------------------------------
#                     INCLUDES DEL MOTOR
# -----------------------------------------------------------

EE_INCS = -I$(INC_DIR) -I$(SRC_DIR)
EE_INCS += -I$(PS2SDK)/ports/include
EE_INCS += -I$(PS2DEV)/gsKit/ee/gs/include -I$(PS2DEV)/gsKit/ee/dma/include

EE_CFLAGS   = -O2 -G0 -Wall -DPS2_BUILD -DPS2_DEBUG $(EE_INCS) 
# Debug
#CFLAGS += -DPS2_BUILD -DPS2_DEBUG

# Release
# CFLAGS += -DPS2_BUILD
EE_CXXFLAGS = $(EE_CFLAGS) -fno-exceptions -fno-rtti

# -----------------------------------------------------------
#                  LIBRERÍAS PS2 SDK
# -----------------------------------------------------------

EE_LIBS = \
	-L$(PS2DEV)/gsKit/lib \
	-lgskit -ldmakit \
	-L$(PS2SDK)/ports/lib \
	-lsdlmixer -lSDL_image -lpng -ljpeg -lSDL \
	-laudsrv \
	-L$(PS2SDK)/ee/lib \
	-lm -lc -lkernel -lstdc++


# -----------------------------------------------------------
#     Incluir toolchain SIN generar reglas automáticas
# -----------------------------------------------------------

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp

# -----------------------------------------------------------
#                  REGLAS DE COMPILACIÓN
# -----------------------------------------------------------

all: $(TARGET)

# Compilar .cpp del motor
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(EE_CXX) -c $< -o $@ $(EE_CXXFLAGS)

# Compilar .c del motor
$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(dir $@)
	$(EE_CC) -c $< -o $@ $(EE_CFLAGS)

# Compilar main de pruebas
$(OBJ_DIR)test_engine.o: $(TEST_MAIN_CPP)
	@mkdir -p $(dir $@)
	$(EE_CXX) -c $< -o $@ $(EE_CXXFLAGS)

# Link final del ELF (Regla oficial PS2SDK)
$(TARGET): $(EE_OBJS)
	$(EE_CC) -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(EE_CXXFLAGS) \
		-o $(TARGET) $(PS2SDK)/ee/startup/crt0.o $(EE_OBJS) $(EE_LDFLAGS) $(EE_LIBS)


# -------------------------
# LIMPIEZA REAL
# -------------------------
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# -------------------------
# REBUILD → limpia + compila
# -------------------------
.PHONY: rebuild
rebuild: clean all

run: $(TARGET)
	ps2client execee host:$(TARGET)

reset:
	ps2client reset

emu: $(TARGET)
	pcsx2 --elf $(TARGET)
