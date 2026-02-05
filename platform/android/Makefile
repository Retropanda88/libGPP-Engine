# === libGPP-Engine — Android Build ===

ROOT_DIR := ../..
SRC_DIR  := $(ROOT_DIR)/src
INC_DIR  := $(ROOT_DIR)/include
TESTS_DIR := $(ROOT_DIR)/tests

BUILD_DIR := build
BIN_DIR := bin

# Nombre del ejecutable por defecto
TARGET := engine_test

# Permitir seleccionar test:
# Ejemplo: make test=test_engine
TEST ?= test_engine

# Main del test seleccionado
MAIN_SRC := $(TESTS_DIR)/$(TEST)/main.cpp
MAIN_OBJ := $(BUILD_DIR)/tests/$(TEST)/main.o

# Compiladores (se sobreescriben con tus exports)
CC  ?= gcc
CXX ?= g++

CFLAGS   = -O3 -Wall -DANDROID_BUILD `sdl-config --cflags` -I$(INC_DIR) -I$(INC_DIR)/engine
CXXFLAGS = -O3 -Wall `sdl-config --cflags` -I$(INC_DIR) -I$(INC_DIR)/engine
LDFLAGS  = `sdl-config --libs` -lSDL_image

# Fuentes del engine
C_SRCS   := $(shell find $(SRC_DIR) -name "*.c")
CPP_SRCS := $(shell find $(SRC_DIR) -name "*.cpp")

OBJS := $(C_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) \
        $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# ====================== TARGETS ======================

all: dirs build_objects link

dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(dir $(MAIN_OBJ))

build_objects: $(OBJS) $(MAIN_OBJ)
	@echo ">> Engine + Test objects compiled."

# Compilar .c del Engine
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar .cpp del Engine
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar main.cpp del test seleccionado
$(MAIN_OBJ): $(MAIN_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

link:
	$(CXX) -o $(BIN_DIR)/$(TARGET) $(OBJS) $(MAIN_OBJ) $(LDFLAGS)
	@echo ">> Ejecutable generado en: $(BIN_DIR)/$(TARGET)"
	@echo ">> Test compilado: $(TEST)"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean dirs build_objects link