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
SRC_FILES = $(filter-out $(SRC_DIR)/plugins/plugin/plugin_bonus.c, $(shell find $(SRC_DIR) -name '*.c'))
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

PLUGIN_SRC = $(wildcard $(SRC_DIR)/plugins/plugin/*.c)
PLUGIN_LIB = $(PLUGIN_SRC:$(SRC_DIR)/plugins/plugin/%.c=plugins/%.so)

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(FFMPEG_INCLUDE_DIR) -c $< -o $@

plugin: $(PLUGIN_LIB)

# Règle pour compiler directement en .so ou .dll sans passer par .o
plugins/%.so: $(SRC_DIR)/plugins/plugin/%.c
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

clean:
	rm -rf $(OBJ_DIR)/*

fclean: clean
	rm -f $(EXEC) plugins/*.so plugins/*.dll

re: fclean all

.PHONY: all clean fclean re plugin plugin_clean
