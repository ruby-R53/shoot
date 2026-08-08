# default flags, change if desired
FLAGS  = -lncurses -ltinfo
FLAGS += -Wall -Werror -Wno-c23-extensions
DEBUG  = -g -fsanitize=address

# so that we get all the necessary
# symbols for compiling
SRCES = art.c shoot.c sprites.c main.c

# and put this all together
all:
	clang -o shoot $(FLAGS) $(SRCES)

# and additionally you can easily
# make debug builds
dbg:
	clang -o shoot $(FLAGS) $(DEBUG) $(SRCES)
