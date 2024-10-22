EXEC = Landtales

CC = gcc

CFLAGS = -Wall -Wextra -std=c99 `sdl-config --cflags`

SRC_DIR = src
OBJ_DIR = object
INCLUDE_DIR = include
CORE_DIR = $(SRC_DIR)/core
EDITOR_DIR = $(SRC_DIR)/editor
GAME_DIR = $(SRC_DIR)/game
MULTIPLAYER_DIR = $(SRC_DIR)/multiplayer
SETTINGS_DIR = $(SRC_DIR)/settings

LIBS = `sdl-config --libs` -lSDL -lSDL_ttf

SRC_FILES = $(CORE_DIR)/main.c \
            $(CORE_DIR)/menu.c \
            $(EDITOR_DIR)/editor.c \
            $(GAME_DIR)/game.c \
            $(MULTIPLAYER_DIR)/multiplayer.c \
            $(SETTINGS_DIR)/settings.c

OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) # Crée le dossier objet s'il n'existe pas
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/*.o

fclean: clean
	rm -f $(EXEC)

re: fclean all

.PHONY: all clean fclean re
