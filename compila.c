#include "dict.h"
#include "codeList.h"
#include <stdio.h>
#include <stdlib.h>
#include "compila.h"

typedef struct compiler{
	CodeList *codes;	// Armazena os códigos de máquina resultantes em uma lista encadeada
	Dict *opCodes;		// Códigos de máquina por operação
	Dict *handlers; 	// Armazena referência às funções de tratamento (atribuicao, retorno, desvio)
	Dict *locals; 		// Armazena posições de cada variável local de acordo com seu índice
	FILE *f; 			 // Ponteiro para arquivo sendo usado na compilação
  Dict *lineDict;   // Armazena para cada linha do progama sb, sua correspondente em assembly
	int line; 			// Identifica a linha atual
  int assemblyLine;
	int varQuant; 		// Contador de variáveis locais utilizadas
  CodeList *ifLines;
}Compiler;

typedef void (*opHandler)(Compiler*); // Protótipo das funções de tratamento

static void error (const char *msg, int line); 	// **
void checkVar(char var, int idx, int line); 	// Assertivas fornecidas pela professora
void checkVarP(char var, int idx, int line);	// **

int getLocal(Compiler *comp,int idx); // Fornece a posição da variável local na pilha dado o seu índice
char *getOp(Compiler *comp,char op);
void setLine(Compiler *comp);

void varHandler(Compiler *comp);	// **
void retHandler(Compiler *comp);	// Funções de tratamento
void ifHandler(Compiler *comp);		// **
void handle(Compiler *comp, char key);

Dict *prepareHandlers();			// Prepara as referências às funções de tratamento
Dict *prepareOps();
Compiler *compiler_init(FILE *f);	// Inicializa o compilador
void compiler_free(Compiler *comp); // Libera memória alocada pelo compilador

//#define FROM_R10D 0x45
//#define TO_R11D 0xd3
//#define SUM 0x01  //MACHINE CODE TO SUM %r10d TO %r11d -> addl %r10d, %r11d
char SUM[4] = {0x45, 0x01, 0xd3, '\0'};
//#define SUB 0X28 //MACHINE CODE TO SUB %r10d from %r11d -> ...
char SUB[4] = {0x45, 0x28, 0xd3, '\0'};
//#define MUL 0x0 //MACHINE CODE TO IMUL %r10d TO %r11d
char MUL[5] = {0x45, 0x0f, 0xaf, 0xda, '\0'};
#define LEAVE 0xc9 //MACHINE CODE TO LEAVE
#define RET 0xc3 //MACHINE CODE TO RET
#define STACK_PUSH 0x55 //TO DO: MACHINE CODE TO: pushq %rbp
const char STACK_MOVE[4] = {0x48, 0x89, 0xe5,'\0'};
const char STACK_SUB[4] = {0X48, 0X83, 0Xec,'\0'};
//#define STACK_MOVE 0x0 //TO DO: MACHINE CODE TO: movq %rsp, %rbp

char COMPA[4] = {0x41, 0x83, 0xfb, '\0'}; //Seguido de numero a se comparar (00 no caso)
char JUMP_EQUAL[3] = {0x0f, 0x84,'\0'}; //Seguido de INT (8 bytes) com linha
char JUMP_GREATER[3] = {0x0f, 0x8f,'\0'}; //Seguido de INT (8 bytes) com linha
char JUMP_LESS[3] = {0x0f, 0x8c,'\0'}; //Seguido de INT (8 bytes) com linha


funcp compila (FILE *f){
	Compiler *comp = compiler_init(f);
	char c;
	char q;
  int i;
	
	while ((c = fgetc(comp->f)) != EOF){
		handle(comp,c);
    	fscanf(comp->f, " ");
  	}
  	q = (((comp->varQuant*4-1)/16)+1)*16;
  	//subq $q, %rsp
    printf("%d\n",q);
    codeList_insertPrepCode(comp->codes,q);
    codeList_insertPrepCodes(comp->codes,STACK_SUB);
  	codeList_insertPrepCodes(comp->codes,STACK_MOVE);
  	codeList_insertPrepCode(comp->codes,STACK_PUSH);
    codeList_insertCode(comp->codes,0x00);
    printf("endCodes\n");
  	char *codes = codeList_toArray(comp->codes);
    for(i=0;i<codeList_getSize(comp->codes);i++)
      printf("%x\n",codes[i]&0xff);
    printf("gotArray\n");
  	compiler_free(comp);
    printf("compilerEnd\n");
  	return (funcp)codes;
}

Dict *prepareHandlers(){
	opHandler handlers[3] = {varHandler,retHandler,ifHandler};
	Dict *d = dict_init(sizeof(int*));
	dict_set(d,"v",&handlers[0]);
	dict_set(d,"r",&handlers[1]);
	dict_set(d,"i",&handlers[2]);
	return d;
}

Dict *prepareOps(){
	Dict *d = dict_init(sizeof(char*));
	dict_set(d,"+",&SUM);
	dict_set(d,"-",&SUB);
	dict_set(d,"*",&MUL);
	return d;
}

Compiler *compiler_init(FILE *f){
	Compiler *comp = (Compiler*)malloc(sizeof(Compiler));
	if(comp==NULL) exit(-1);
	comp->handlers = prepareHandlers();
	comp->codes = codeList_init();
  comp->ifLines = codeList_init();
	comp->locals = dict_init(sizeof(int));
  comp->lineDict = dict_init(sizeof(int));
  comp->opCodes = prepareOps();
	comp->f = f;
	comp->varQuant = 0;
	comp->line = 1;
  comp->assemblyLine = 1;
	return comp;
}

void compiler_free(Compiler *comp){
	codeList_free(comp->codes);
  codeList_free(comp->ifLines);
	dict_free(comp->opCodes);
	dict_free(comp->handlers);
	dict_free(comp->locals);
  dict_free(comp->lineDict);
	free(comp);
}

void handle(Compiler *comp, char key){
	char k[2] = {key,'\0'};
	(*((opHandler*)dict_getValue(comp->handlers,k)))(comp);
	comp->line++;
}

void retHandler(Compiler *comp){
  printf("retorno\n");
	int idx, space;
  char var;
  if (fscanf(comp->f, "et %c%d", &var, &idx) != 2)
  	error("comando invalido", comp->line);
  if (var == '$'){
  	//checkVarP(var, idx, comp->line);
    codeList_insertCode(comp->codes,0xb8);
    codeList_insertInt(comp->codes,idx);
  	// CODIGO DE MAQUINA movl $idx, %eax
  }
  else{
  	space = getLocal(comp,idx);
    codeList_insertCode(comp->codes,0x8b);
    codeList_insertCode(comp->codes,0x45);
    codeList_insertCode(comp->codes,256-4*space);
    // CODIGO DE MAQUINA movl -4*space(%rbp), %eax
  }
  codeList_insertCode(comp->codes,LEAVE);
  codeList_insertCode(comp->codes,RET);
  //printf("ret %c%d\n", var, idx);
  setLine(comp);
  comp->assemblyLine += 3;
  printf("end retorn\n");
}

void varHandler(Compiler *comp){
  printf("atribuicao\n");
	int idx0, idx1, idx2,space;
  char var1, var2;//var0 = c, var1, var2;
  char op;
  if (fscanf(comp->f, "%d = %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2, &idx2) != 6)
  	error("comando invalido", comp->line);
  //checkVar(var0, idx0, comp->line);
  if (var1 == '$'){
  	//checkVarP(var1, idx1, comp->line);
    codeList_insertCode(comp->codes,0x41);
    codeList_insertCode(comp->codes,0xbb);
    codeList_insertInt(comp->codes,idx1);
  	// CODIGO DE MAQUINA movl $idx1, %r11d
  }
  else if(var1 == 'v'){
  	space = getLocal(comp,idx1);
    codeList_insertCode(comp->codes,0x44);
    codeList_insertCode(comp->codes,0x8b);
    codeList_insertCode(comp->codes,0x5d);
    codeList_insertCode(comp->codes,256-4*space);
  	// CODIGO DE MAQUINA movl -4*space(%rpb), %r11d
  }
  else{
    codeList_insertCode(comp->codes,0x41);
    codeList_insertCode(comp->codes,0x89);
    codeList_insertCode(comp->codes,(idx1==0)?(0xfb):(idx1==1)?(0xf3):(0xd3));
  }
  if (var2 == '$'){
  	//checkVarP(var2, idx2, comp->line);
    codeList_insertCode(comp->codes,0x41);
    codeList_insertCode(comp->codes,0xba);
    codeList_insertInt(comp->codes,idx2);
  	// CODIGO DE MAQUINA movl $idx2, %r10d
  }
  else if(var2 == 'v'){
  	space = getLocal(comp,idx2);
    codeList_insertCode(comp->codes,0x44);
    codeList_insertCode(comp->codes,0x8b);
    codeList_insertCode(comp->codes,0x55);
    codeList_insertCode(comp->codes,256-4*space);
  	// CODIGO DE MAQUINA movl -4*space(%rpb), %r10d
  }
  else{
    codeList_insertCode(comp->codes,0x41);
    codeList_insertCode(comp->codes,0x89);
    codeList_insertCode(comp->codes,(idx2==0)?(0xfa):(idx2==1)?(0xf2):(0xd2));
  }

  //codeList_insert(comp->codes,FROM_R10D);
  codeList_insertCodes(comp->codes,getOp(comp,op)); //Code for operation
  //codeList_insert(comp->codes,TO_R11D);
  space = getLocal(comp,idx0);  //Assigning the result
  codeList_insertCode(comp->codes,0x44);
  codeList_insertCode(comp->codes,0x89);
  codeList_insertCode(comp->codes,0x5d);
  codeList_insertCode(comp->codes,256-4*space);
  // CODIGO DE MAQUINA X = movl %r11d, -4*space(%rbp)
  // codeList_insert(comp->codes,X);
  //printf("%c%d = %c%d %c %c%d\n",var0, idx0, var1, idx1, op, var2, idx2);
  setLine(comp);
  comp->assemblyLine += 4;
  printf("fim atribuicao\n");
}

void ifHandler(Compiler *comp){
	int idx, n1, n2, n3;
  char var;
  if (fscanf(comp->f, "f %c%d %d %d %d", &var, &idx, &n1, &n2, &n3) != 5)
  error("comando invalido", comp->line);
  if (var != '$') checkVar(var, idx, comp->line);

  codeList_insertCodes(comp->codes,COMPA);
  codeList_insertCode(comp->codes,0);

  setLine(comp);
  
  printf("if %c%d %d %d %d\n", var, idx, n1, n2, n3);
}

void setLine(Compiler *comp){
  char key[3];
  sprintf(key,"%d",comp->line);
  dict_set(comp->lineDict,key,&(comp->assemblyLine));
}

int getLocal(Compiler *comp,int idx){
	char key[3];
	int *val;
	sprintf(key,"%d",idx);
	val = (int*)dict_getValue(comp->locals,key);
    if(val==NULL){ //checking if this variable was used before
      ++comp->varQuant;
    	dict_set(comp->locals,key,&(comp->varQuant));
    	return comp->varQuant;
    }
    return *val;
}

char *getOp(Compiler *comp,char op){
  //char opp[2] = {op,'\0'};
  //char *ch = *((char **)dict_getValue(comp->opCodes,opp));
  //return ch;
  return (op=='+') ? SUM : (op=='-') ? SUB : MUL;
}

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(-1);
}

void checkVar(char var, int idx, int line) {
  switch (var) {
    case 'v':
      if ((idx < 0) || (idx > 19))
       error("operando invalido", line);
      break;
    default:
       error("operando invalido", line);
   }
}
     
void checkVarP(char var, int idx, int line) {
  switch (var) {
    case 'v':
      if ((idx < 0) || (idx > 19))
       error("operando invalido", line);
      break;
    case 'p':
      if ((idx < 0) || (idx > 2))
       error("operando invalido", line);
      break;
    default:
       error("operando invalido", line);
   }
}