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
CXXFLAGS = -O3 -Wall  -DANDROID_BUILD `sdl-config --cflags` -I$(INC_DIR) -I$(INC_DIR)/engine
LDFLAGS  = `sdl-config --libs` -lSDL_image -lSDL_ttf

# ====================== BÚSQUEDA DE FUENTES ======================

# 1. Fuentes del Engine (src/)
ENGINE_C_SRCS   := $(shell find $(SRC_DIR) -name "*.c")
ENGINE_CPP_SRCS := $(shell find $(SRC_DIR) -name "*.cpp")

# 2. Fuentes adicionales dentro del Test activo (excluyendo main.cpp para tratarlo aparte)
TEST_EXTRA_C   := $(shell find $(TESTS_DIR)/$(TEST) -name "*.c" ! -name "main.c")
TEST_EXTRA_CPP := $(shell find $(TESTS_DIR)/$(TEST) -name "*.cpp" ! -name "main.cpp")

# Mapeo de objetos del Engine
ENGINE_OBJS := $(ENGINE_C_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/engine/%.o) \
               $(ENGINE_CPP_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/engine/%.o)

# Mapeo de objetos adicionales del Test (UIElement.cpp, UIText.cpp, etc.)
TEST_OBJS := $(TEST_EXTRA_C:$(TESTS_DIR)/$(TEST)/%.c=$(BUILD_DIR)/tests/$(TEST)/%.o) \
             $(TEST_EXTRA_CPP:$(TESTS_DIR)/$(TEST)/%.cpp=$(BUILD_DIR)/tests/$(TEST)/%.o)

OBJS := $(ENGINE_OBJS) $(TEST_OBJS)

# ====================== TARGETS ======================

all: dirs build_objects link

dirs:
	@mkdir -p $(BUILD_DIR)/engine
	@mkdir -p $(BUILD_DIR)/tests/$(TEST)
	@mkdir -p $(BIN_DIR)

build_objects: $(OBJS) $(MAIN_OBJ)
	@echo ">> Engine + Test objects (incluyendo clases UI) compilados con éxito."

# Compilar .c del Engine
$(BUILD_DIR)/engine/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar .cpp del Engine
$(BUILD_DIR)/engine/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar ficheros .cpp adicionales del Test (UIElement, UIText, etc.)
$(BUILD_DIR)/tests/$(TEST)/%.o: $(TESTS_DIR)/$(TEST)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar ficheros .c adicionales del Test (si los hubiera)
$(BUILD_DIR)/tests/$(TEST)/%.o: $(TESTS_DIR)/$(TEST)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

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
