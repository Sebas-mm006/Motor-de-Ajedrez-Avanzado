# Makefile simple para compilar el motor de ajedrez
# Cada sección está explicada en español para facilitar su comprensión

# =============================================
# CONFIGURACIÓN DEL COMPILADOR Y BANDERAS
# =============================================

# Compilador a utilizar (g++ para C++)
CXX = g++

# Banderas de compilación:
# -std=c++17: Usa el estándar C++17
# -Wall: Activa la mayoría de las advertencias útiles
# -Wextra: Activa advertencias adicionales
# -Iinclude: Le dice al compilador dónde buscar los archivos de encabezado (.hpp)
# -g: Incluye información de depuración (útil para gdb)
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -g

# Banderas de enlazado (en este caso no necesitamos ninguna especial)
LDFLAGS =

# =============================================
# CONFIGURACIÓN DEL PROYECTO
# =============================================

# Nombre del ejecutable final que se generará
TARGET = bin/chess

# Directorios de fuentes y objetos
SRC_DIR = src
OBJ_DIR = obj

# Lista explícita de todos los archivos fuente .cpp
# (Es mejor listarlos explícitamente que usar wildcards para mayor control)
SOURCES = \
    $(SRC_DIR)/Bishop.cpp \
    $(SRC_DIR)/Board.cpp \
    $(SRC_DIR)/Eval.cpp \
    $(SRC_DIR)/King.cpp \
    $(SRC_DIR)/Knight.cpp \
    $(SRC_DIR)/NullPiece.cpp \
    $(SRC_DIR)/Pawn.cpp \
    $(SRC_DIR)/Piece.cpp \
    $(SRC_DIR)/Queen.cpp \
    $(SRC_DIR)/Rook.cpp \
    $(SRC_DIR)/Search.cpp \
    $(SRC_DIR)/move.cpp \
    $(SRC_DIR)/main.cpp

# Convierte la lista de fuentes a la lista correspondiente de objetos
# Ejemplo: src/Bishop.cpp -> obj/Bishop.o
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# =============================================
# REGLAS DE COMPILACIÓN
# =============================================

# La regla por defecto (la primera) compila el ejecutable principal
all: $(TARGET)

# Regla para crear el ejecutable final enlazando todos los objetos
$(TARGET): $(OBJECTS)
	@echo "Enlazando objetos para crear el ejecutable..."
	@mkdir -p $(dir $@)  # Crea el directorio bin/ si no existe
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Ejecutable creado: $@"

# Regla genérica para compilar cualquier archivo .cpp a .o
# $< representa el prerequisito (el archivo .cpp)
# $@ representa el objetivo (el archivo .o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compilando: $<"
	@mkdir -p $(dir $@)  # Crea el subdirectorio en obj/ si es necesario
	$(CXX) $(CXXFLAGS) -c $< -o $@

# =============================================
# REGLAS DE LIMPIEZA Y UTILIDADES
# =============================================

# Elimina todos los archivos generados (objetos y ejecutable)
clean:
	@echo "Eliminando archivos generados..."
	@rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Limpieza completada."

# Objetos que no representan archivos reales
.PHONY: all clean

# =============================================
# INSTRUCCIONES DE USO
# =============================================
#
# Para compilar normalmente:
#   make
#
# Para forzar una recompilación completa:
#   make clean && make
#
# Para solo limpiar:
#   make clean
#
# El ejecutable resultante estará en: bin/chess
#
# Nota: Las advertencias sobre variables no usadas son normales en algunos
#       archivos como NullPiece.cpp (que tiene parámetros que no usa porque
#       debe seguir la interfaz de las otras piezas). No afectan la funcionalidad.