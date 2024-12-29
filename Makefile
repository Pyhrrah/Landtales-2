EXEC = Landtales

CC = gcc -g

CFLAGS = -Wall -Wextra -std=c99 -finput-charset=UTF-8 -fexec-charset=UTF-8 `sdl2-config --cflags -Wno-char-subscripts` -DSDL_MAIN_HANDLED

SRC_DIR = src
OBJ_DIR = object
INCLUDE_DIR = include

LIBS = `sdl2-config --libs` -lSDL2_ttf -lSDL2_image -lSDL2_net -lm 

# Trouve automatiquement tous les fichiers source
SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
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
