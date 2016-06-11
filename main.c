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
  char pal[4] = {'a',0x00,'b','\0'};
  int i;
  if ((myfp = fopen ("programa.txt", "r")) == NULL) {
    perror ("nao conseguiu abrir arquivo!");
    exit(1);
  }
  printf("Abrindo");
  func = compila(myfp);
  func(2,3,4);
  printf("Completo");
  //printf("%d", func(2,3));
  return 0;
}
