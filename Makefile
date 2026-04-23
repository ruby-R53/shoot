FLAGS = -Wall -lncurses -ltinfo
CODES = shoot.c sprites.c main.c

ifdef DBG
	FLAGS += -fsanitize=address -g
endif

all:
	gcc $(FLAGS) $(CODES) -o shoot
