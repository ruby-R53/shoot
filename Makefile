# default flags, change if desired
FLAGS = -Wall -Wno-main -ltinfo

# so that we get all the necessary
# symbols for compiling
SRCES = art.c shoot.c sprites.c main.c

# and put this all together
all:
	gcc $(FLAGS) $(SRCES) -lncurses -o shoot

# additionally you can easily make
# debug builds instead
dbg:
	gcc $(FLAGS) -fsanitize=address -g $(SRCES) -lncurses -o shoot
