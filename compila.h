#include <stdio.h>

typedef int (*funcp) ();

funcp compila (FILE *f);

void libera (void *p);