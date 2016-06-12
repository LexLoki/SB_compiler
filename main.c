#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compila.h"
    
int teste(void);

int main(void){
  teste();
  return 0;
}

int teste (void) {
  
  FILE *myfp;
  funcp func;
  if ((myfp = fopen ("programa.txt", "r")) == NULL) {
    perror ("nao conseguiu abrir arquivo!");
    exit(1);
  }

  func = compila(myfp);
  printf("Termo 7 de fibbonaci: %d", func(7));

  libera(func);

  return 0;
}
