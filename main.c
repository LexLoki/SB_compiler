#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"
    
int teste(void);

int main(void){
  
  return 0;
}
int teste (void) {
  
  int line = 1;
  int  c;
  FILE *myfp;

  if ((myfp = fopen ("programa", "r")) == NULL) {
    perror ("nao conseguiu abrir arquivo!");
    exit(1);
  }

  //compila(myfp);
  return 0;
}
