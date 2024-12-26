EXEC = Landtales

CC = gcc -g

CFLAGS = -Wall -Wextra -std=c99 -finput-charset=UTF-8 -fexec-charset=UTF-8 `sdl2-config --cflags -Wno-char-subscripts` 

SRC_DIR = src
OBJ_DIR = object
INCLUDE_DIR = include
CORE_DIR = $(SRC_DIR)/core
EDITOR_DIR = $(SRC_DIR)/editor
GAME_DIR = $(SRC_DIR)/game
MULTIPLAYER_DIR = $(SRC_DIR)/multiplayer
SETTINGS_DIR = $(SRC_DIR)/settings
LOGIC_DIR = $(SRC_DIR)/logic

LIBS = `sdl2-config --libs` -lSDL2_ttf -lSDL2_image -lSDL2_net -lm -DSDL_MAIN_HANDLED

SRC_FILES = $(CORE_DIR)/main.c \
            $(CORE_DIR)/menu.c \
            $(CORE_DIR)/player.c \
            $(CORE_DIR)/ennemies.c \
            $(EDITOR_DIR)/editor.c \
            $(EDITOR_DIR)/map_test.c \
			$(EDITOR_DIR)/colors.c \
            $(EDITOR_DIR)/grid.c \
            $(GAME_DIR)/game.c \
            $(GAME_DIR)/map.c \
            $(GAME_DIR)/pause.c \
            $(MULTIPLAYER_DIR)/multiplayer.c \
            $(MULTIPLAYER_DIR)/server.c \
            $(MULTIPLAYER_DIR)/client.c \
            $(SETTINGS_DIR)/settings.c \
            $(LOGIC_DIR)/check_maze.c \


OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/*

fclean: clean
	rm -f $(EXEC)

re: fclean all

.PHONY: all clean fclean re
