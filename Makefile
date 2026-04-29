# default flags, change if desired
FLAGS = -Wall -Wno-main -lncurses -ltinfo

# so that we get all the necessary
# symbols for compiling
SRCES = shoot.c sprites.c main.c

# and put this all together
all:
	gcc $(FLAGS) $(SRCES) -o shoot

# additionally you can easily make
# debug builds instead
dbg:
	gcc $(FLAGS) -fsanitize=address -g $(SRCES) -o shoot
