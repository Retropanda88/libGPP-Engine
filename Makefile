#
# Makefile maestro de libGPP-Engine
#

# Plataformas soportadas
#PLATFORMS := pc gc ps2 psp android
PLATFORMS :=  gc ps2 psp 

# Mensaje por defecto
help:
	@echo "Uso:"
	@echo "  make <plataforma>"
	@echo ""
	@echo "Plataformas disponibles:"
	@echo "  pc        Compila para PC"
	@echo "  gc        Compila para GameCube"
	@echo "  ps2       Compila para PlayStation 2"
	@echo "  psp       Compila para PSP"
	@echo "  android   Compila para Android"
	@echo ""
	@echo "Ejemplo:"
	@echo "  make pc"
	@echo "  make gc"

# Plataforma PC
#pc:
#	@echo "=== Compilando libGPP-Engine para PC ==="
#	$(MAKE) -C platform/pc

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

# Android
#android:
#	@echo "=== Compilando libGPP-Engine para Android ==="
#	$(MAKE) -C platform/android

# Compilar todo
all:
	@echo "=== Compilando TODAS las plataformas ==="
	@for p in $(PLATFORMS); do \
		$(MAKE) $$p; \
	done

# Limpiar todos los builds
clean:
	rm -rf build/*
	rm -rf bin/*
	#$(MAKE) -C platform/android clean
	#$(MAKE) -C platform/pc clean
	$(MAKE) -C platform/ps2 clean
	$(MAKE) -C platform/gamecube clean
	$(MAKE) -C platform/psp clean
