# default flags, change if desired
FLAGS = -Wall -Wno-main -lncurses -ltinfo

# so that we get all the necessary
# symbols for compiling
SRCES = art.c shoot.c sprites.c main.c

# and put this all together
all:
	gcc $(SRCES) -o shoot $(FLAGS)

# additionally you can easily make
# debug builds instead
dbg:
	gcc -g $(SRCES) -o shoot $(FLAGS)
