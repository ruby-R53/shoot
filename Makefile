# default flags, change if desired
FLAGS  = -lncurses -ltinfo
FLAGS += -Wall -Werror -pedantic
DEBUG  = -g -fsanitize=address

# so that we get all the necessary
# symbols for compiling
SRCES = art.c shoot.c sprites.c main.c

# and put this all together
build:
	clang -o shoot $(FLAGS) $(SRCES)

# additionally strip if wanted
strip: build
	strip -s shoot

# and additionally you can easily
# make debug builds
dbg:
	clang -o shoot $(FLAGS) $(DEBUG) $(SRCES)
