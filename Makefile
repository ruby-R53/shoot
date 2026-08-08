# default flags, change if desired
FLAGS = -Werror -Wall -Wno-c23-extensions -lncurses -ltinfo
DEBUG = -g -fsanitize=address

# so that we get all the necessary
# symbols for compiling
SRCES = art.c shoot.c sprites.c main.c

# and put this all together
all:
	clang $(SRCES) -o shoot $(FLAGS)

# additionally you can easily make
# debug builds instead
dbg:
	clang $(SRCES) -o shoot $(FLAGS) $(DEBUG)
