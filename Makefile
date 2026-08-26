#
# Makefile maestro de libGPP-Engine
#

# Plataformas soportadas
PLATFORMS := nes gc ps2 psp 

# Mensaje por defecto
help:
	@echo "Uso:"
	@echo "  make <plataforma>"
	@echo ""
	@echo "Plataformas disponibles:"
	@echo "  gc         Compila para GameCube"
	@echo "  ps2        Compila para PlayStation 2"
	@echo "  psp        Compila para PSP"
	@echo "  nes        Compila la librería NES (Multi-plataforma)"
	@echo ""
	@echo "Ejemplo:"
	@echo "  make gc"
	@echo "  make nes"

# GameCube
gc:
	@echo "=== Compilando libGPP-Engine para GameCube ==="
	$(MAKE) -C platform/gamecube

# PlayStation 2
ps2:
	@echo "=== Compilando libGPP-Engine para PS2 ==="
	$(MAKE) -C platform/ps2

ps2-run:
	@echo "=== Ejecutando PS2 ==="
	$(MAKE) -C platform/ps2 run

ps2-reset:
	@echo "=== Reset PS2 ==="
	$(MAKE) -C platform/ps2 reset

# PSP
psp:
	@echo "=== Compilando libGPP-Engine para PSP ==="
	$(MAKE) -C platform/psp

# Nintendo NES (Compila las variantes de la librería)
nes:
	@echo "=== Compilando Librería NES para PS2 ==="
	$(MAKE) -C nes -f Makefile.ps2
	@echo "=== Compilando Librería NES para PSP ==="
	$(MAKE) -C nes -f Makefile.psp
	@echo "=== Compilando Librería NES para GameCube ==="
	$(MAKE) -C nes -f Makefile.gc

# Compilar todo
all:
	@echo "=== Compilando TODAS las plataformas ==="
	@for p in $(PLATFORMS); do \
		$(MAKE) $$p; \
	done

# Limpiar todos los builds
clean:
	@echo "=== Limpiando todos los builds ==="
	rm -rf build/*
	rm -rf bin/*
	$(MAKE) -C platform/gamecube clean
	$(MAKE) -C platform/ps2 clean
	$(MAKE) -C platform/psp clean
	@echo "=== Limpiando sub-librerías NES ==="
	$(MAKE) -C nes -f Makefile.ps2 clean
	$(MAKE) -C nes -f Makefile.psp clean
	$(MAKE) -C nes -f Makefile.gc clean

.PHONY: help gc ps2 ps2-run ps2-reset psp nes all clean