#include "shoot.h"

typedef struct ART {
	WINDOW* win;
	int rows, cols;
	const char* art[]; // the art itself
} ART;

extern void printart(ART* art, int y, int x);

extern ART title;
