FLAGS = -Wall -lncurses -ltinfo

ifdef DBG
	FLAGS += -fsanitize=address -g
endif

all:
	gcc $(FLAGS) shoot.c main.c -o shoot
