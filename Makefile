EXEC = Landtales

CC = gcc -g

CFLAGS = -Wall -Wextra -std=c99 -finput-charset=UTF-8 -fexec-charset=UTF-8 `sdl2-config --cflags -Wno-char-subscripts` -DSDL_MAIN_HANDLED

SRC_DIR = src
OBJ_DIR = object
INCLUDE_DIR = include
FFMPEG_INCLUDE_DIR = /path/to/ffmpeg/include
FFMPEG_LIB_DIR = /path/to/ffmpeg/lib

LIBS = `sdl2-config --libs` -lSDL2_ttf -lSDL2_image -lSDL2_net -lSDL2_mixer -lm -L/usr/lib -lavformat -lavcodec -lavutil -lswresample

# Exclure explicitement les fichiers dans /src/plugins/plugin/ de la compilation principale
SRC_FILES = $(filter-out $(SRC_DIR)/plugins/plugin/%, $(shell find $(SRC_DIR) -name '*.c'))
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Fichiers sources des plugins
PLUGIN_SRC = $(wildcard $(SRC_DIR)/plugins/plugin/*.c)
PLUGIN_LIB = $(PLUGIN_SRC:$(SRC_DIR)/plugins/plugin/%.c=plugins/%.so)

# Cible principale
all: $(EXEC)

# Lien des fichiers objets pour créer l'exécutable
$(EXEC): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LIBS)

# Compilation des fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(FFMPEG_INCLUDE_DIR) -c $< -o $@

# Cible pour compiler les plugins
plugin: $(PLUGIN_LIB)

# Règle pour compiler les plugins en .so ou .dll
plugins/%.so: $(SRC_DIR)/plugins/plugin/%.c
	@mkdir -p plugins
ifeq ($(OS), Windows_NT)
	# Si on est sous Windows, créer un fichier DLL
	$(CC) -shared -o $@ $^ $(LIBS) -fPIC
else
	# Si on est sous Linux ou autre, créer un fichier .so
	$(CC) -shared -o $@ $^ $(LIBS) -fPIC
endif

# Commande pour nettoyer les plugins
plugin_clean:
	rm -f plugins/*.so plugins/*.dll

# Commande pour nettoyer les fichiers objets
clean:
	rm -rf $(OBJ_DIR)/*

# Commande pour nettoyer tout (fichiers objets et exécutable)
fclean: clean
	rm -f $(EXEC) plugins/*.so plugins/*.dll

# Commande pour tout recompiler
re: fclean all

.PHONY: all clean fclean re plugin plugin_clean