#include "dict.h"
#include "codeList.h"
#include "jumpList.h"
#include "compila.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct compiler{
	CodeList *codes;	// Armazena os códigos de máquina resultantes em uma lista encadeada
	//Dict *opCodes;		// Códigos de máquina por operação
	Dict *handlers; 	// Armazena referência às funções de tratamento (atribuicao, retorno, desvio)
	Dict *locals; 		// Armazena posições de cada variável local de acordo com seu índice
	FILE *f; 			    // Ponteiro para arquivo sendo usado na compilação
  Dict *lineDict;   // Armazena para cada linha do progama sb, o índice do primeiro código de máquina dela
	int line; 			  // Identifica a linha atual do codigo SB
  int assemblyLine; // Identifica o codigo de maquina (byte) atual correspondente ao SB atual (line)
	int varQuant; 		// Contador de variáveis locais utilizadas
  JumpNode *jumpCodes;  // Lista encadeada com nós apontando para o começo dos códigos de máquina que especificam o valor dos jumps.
}Compiler;

static void error (const char *msg, int line);     // Função para erro
static void printCodes (const char *codes, int n); // Função para mostrar os códigos de maquina

typedef void (*opHandler)(Compiler*); // Protótipo das funções de tratamento

int getLocal(Compiler *comp,int idx); // Fornece a posição da variável local na pilha dado o seu índice
const char *getOp(Compiler *comp,char op);  // Obtem os códigos de máquina referente a uma dada operação

void setLine(Compiler *comp); // Relaciona a linha atual SB com o índice do próximo código de máquina

void varHandler(Compiler *comp);  // **
void retHandler(Compiler *comp);  // Funções de tratamento
void ifHandler(Compiler *comp);	  // **
void handle(Compiler *comp, char key); // Iterador que chama função de tratamento

Dict *prepareHandlers();  // Prepara as referências às funções de tratamento
//Dict *prepareOps();       // Prepara os códigos das operações
Compiler *compiler_init(FILE *f);	// Inicializa o compilador
void compiler_free(Compiler *comp); // Libera memória alocada pelo compilador

const char SUM[4] = {0x45, 0x01, 0xd3, '\0'};       // MACHINE CODE TO SUM %r10d TO %r11d -> addl %r10d, %r11d
const char SUB[4] = {0x45, 0x29, 0xd3, '\0'};       // MACHINE CODE TO SUB %r10d from %r11d
const char MUL[5] = {0x45, 0x0f, 0xaf, 0xda, '\0'}; // MACHINE CODE TO IMUL %r10d TO %r11d

#define LEAVE 0xc9      // MACHINE CODE TO LEAVE
#define RET 0xc3        // MACHINE CODE TO RET
#define STACK_PUSH 0x55 // MACHINE CODE TO: pushq %rbp

const char STACK_MOVE[4] = {0x48, 0x89, 0xe5,'\0'}; // MACHINE CODE TO: movq %rsp, %rbp
const char STACK_SUB[4] = {0X48, 0X83, 0Xec,'\0'};  // Para espaço na pilha (seguido de constante)

const char COMPA[4] = {0x41, 0x83, 0xfb, '\0'}; // Seguido de numero a se comparar (00 no caso) %r11d
const char JUMP_EQUAL[3] = {0x0f, 0x84,'\0'};   // Seguido de INT (8 bytes) com linha
const char JUMP_GREATER[3] = {0x0f, 0x8f,'\0'}; // Seguido de INT (8 bytes) com linha
const char JUMP_LESS[3] = {0x0f, 0x8c,'\0'};    // Seguido de INT (8 bytes) com linha


funcp compila (FILE *f){
	Compiler *comp = compiler_init(f);
	char c,q;
  int n;
	while ((c = fgetc(comp->f)) != EOF){
		handle(comp,c);
    fscanf(comp->f, " ");
  }
	q = (((comp->varQuant*4-1)/16)+1)*16;
  codeList_insertPrepCode(comp->codes,q);
  codeList_insertPrepCodes(comp->codes,STACK_SUB);
	codeList_insertPrepCodes(comp->codes,STACK_MOVE);
	codeList_insertPrepCode(comp->codes,STACK_PUSH);

  n = codeList_getSize(comp->codes);
  jumpList_prepJumps(comp->jumpCodes,comp->lineDict); //prepare jumps

	char *codes = codeList_toArray(comp->codes);
  //printCodes(codes,n);
	compiler_free(comp);
	return (funcp)codes;
}

void libera (void *p){
  free((char*)p);
}

static void printCodes (const char *codes, int n){
  int i;
  for(i=0;i<n;i++)
    printf("%x ",codes[i]&0xff);
  printf("\n");
}

Dict *prepareHandlers(){
	opHandler handlers[3] = {varHandler,retHandler,ifHandler};
	Dict *d = dict_init(sizeof(int*));
	dict_set(d,"v",&handlers[0]);
	dict_set(d,"r",&handlers[1]);
	dict_set(d,"i",&handlers[2]);
	return d;
}

// Dict *prepareOps(){
// 	Dict *d = dict_init(sizeof(char*));
// 	dict_set(d,"+",&SUM);
// 	dict_set(d,"-",&SUB);
// 	dict_set(d,"*",&MUL);
// 	return d;
// }

Compiler *compiler_init(FILE *f){
	Compiler *comp = (Compiler*)malloc(sizeof(Compiler));
	if(comp==NULL) exit(-1);
	comp->handlers = prepareHandlers();
	comp->codes = codeList_init();
  comp->jumpCodes = jumpList_init();
	comp->locals = dict_init(sizeof(int));
  comp->lineDict = dict_init(sizeof(int));
  //comp->opCodes = prepareOps();
	comp->f = f;
	comp->varQuant = 0;
	comp->line = 1;
  comp->assemblyLine = 8; //8 primeiros bytes são referentes a preparação da pilha
	return comp;
}

void compiler_free(Compiler *comp){
	codeList_free(comp->codes);
  jumpList_free(comp->jumpCodes);
	//dict_free(comp->opCodes);
	dict_free(comp->handlers);
	dict_free(comp->locals);
  dict_free(comp->lineDict);
	free(comp);
}

void handle(Compiler *comp, char key){
	char k[2] = {key,'\0'};
  setLine(comp);
	(*((opHandler*)dict_getValue(comp->handlers,k)))(comp);
	comp->line++;
}

void retHandler(Compiler *comp){
	int idx, space;
  char var;
  if (fscanf(comp->f, "et %c%d", &var, &idx) != 2)
  	error("comando invalido", comp->line);
  if (var == '$'){
    codeList_insertCode(comp->codes,0xb8);
    codeList_insertInt(comp->codes,idx); //movl $idx, %eax
    comp->assemblyLine += 5;
  }
  else if(var == 'v'){
  	space = getLocal(comp,idx);
    codeList_insertCode(comp->codes,0x8b);
    codeList_insertCode(comp->codes,0x45);
    codeList_insertCode(comp->codes,256-4*space); //movl -4*space(%rbp), %eax
    comp->assemblyLine += 3;
  }
  else{
    codeList_insertCode(comp->codes,0x89);
    codeList_insertCode(comp->codes,(idx==0)?(0xf8):(idx==1)?(0xf0):(0xd0));
    comp->assemblyLine += 2;
  }
  codeList_insertCode(comp->codes,LEAVE);
  codeList_insertCode(comp->codes,RET);
  comp->assemblyLine += 2;
}

void varHandler(Compiler *comp){
	int idx0, idx1, idx2,space;
  char var1, var2;//var0 = c, var1, var2;
  char op;
  if (fscanf(comp->f, "%d = %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2, &idx2) != 6)
  	error("comando invalido", comp->line);
  if (var1 == '$'){
    codeList_insertCode(comp->codes,0x41);
    codeList_insertCode(comp->codes,0xbb);
    codeList_insertInt(comp->codes,idx1); //movl $idx1, %r11d
    comp->assemblyLine += 6;
  }
  else if(var1 == 'v'){
  	space = getLocal(comp,idx1);
    codeList_insertCode(comp->codes,0x44);
    codeList_insertCode(comp->codes,0x8b);
    codeList_insertCode(comp->codes,0x5d);
    codeList_insertCode(comp->codes,256-4*space); //movl -4*space(%rpb), %r11d
    comp->assemblyLine += 4;
  }
  else{
    codeList_insertCode(comp->codes,0x41);
    codeList_insertCode(comp->codes,0x89);
    codeList_insertCode(comp->codes,(idx1==0)?(0xfb):(idx1==1)?(0xf3):(0xd3));
    comp->assemblyLine += 3;
  }
  if (var2 == '$'){
    codeList_insertCode(comp->codes,0x41);
    codeList_insertCode(comp->codes,0xba);
    codeList_insertInt(comp->codes,idx2); //movl $idx2, %r10d
    comp->assemblyLine += 6;
  }
  else if(var2 == 'v'){
  	space = getLocal(comp,idx2);
    codeList_insertCode(comp->codes,0x44);
    codeList_insertCode(comp->codes,0x8b);
    codeList_insertCode(comp->codes,0x55);
    codeList_insertCode(comp->codes,256-4*space); //movl -4*space(%rpb), %r10d
    comp->assemblyLine += 4;
  }
  else{
    codeList_insertCode(comp->codes,0x41);
    codeList_insertCode(comp->codes,0x89);
    codeList_insertCode(comp->codes,(idx2==0)?(0xfa):(idx2==1)?(0xf2):(0xd2));
    comp->assemblyLine += 3;
  }

  codeList_insertCodes(comp->codes,getOp(comp,op)); //Code for operation
  comp->assemblyLine += (op=='*') ? 4 : 3;
  space = getLocal(comp,idx0);  //Assigning the result
  codeList_insertCode(comp->codes,0x44);
  codeList_insertCode(comp->codes,0x89);
  codeList_insertCode(comp->codes,0x5d);
  codeList_insertCode(comp->codes,256-4*space); //movl %r11d, -4*space(%rbp)
  comp->assemblyLine += 4;
}

void ifHandler(Compiler *comp){
	int idx, n1, n2, n3, space;
  char var;
  if (fscanf(comp->f, "f %c%d %d %d %d", &var, &idx, &n1, &n2, &n3) != 5)
    error("comando invalido", comp->line);
  if(var=='$'){
    codeList_insertCode(comp->codes,0x41);codeList_insertCode(comp->codes,0xbb);
    codeList_insertInt(comp->codes,idx);
    comp->assemblyLine += 6; //6 bytes
  }
  else if(var=='v'){
    space = getLocal(comp,idx);
    codeList_insertCode(comp->codes,0x44);codeList_insertCode(comp->codes,0x8b);codeList_insertCode(comp->codes,0x5d);
    codeList_insertCode(comp->codes,256-4*space);
    comp->assemblyLine += 4; //4 bytes
  }
  else{
    codeList_insertCode(comp->codes,0x41);codeList_insertCode(comp->codes,0x89);
    codeList_insertCode(comp->codes,(idx==0)?(0xfb):(idx==1)?(0xf3):(0xd3));
    comp->assemblyLine += 3; //3 bytes
  }

  codeList_insertCodes(comp->codes,COMPA); //3 bytes
  codeList_insertCode(comp->codes,0); //1 byte
  
  comp->assemblyLine += 10;
  codeList_insertCodes(comp->codes,JUMP_LESS); //2 bytes
  comp->jumpCodes = jumpList_insertCodeNode(comp->jumpCodes,codeList_insertJumpCode(comp->codes,n1),comp->assemblyLine); //4 bytes

  comp->assemblyLine += 6;
  codeList_insertCodes(comp->codes,JUMP_EQUAL); //2 bytes
  comp->jumpCodes = jumpList_insertCodeNode(comp->jumpCodes,codeList_insertJumpCode(comp->codes,n2),comp->assemblyLine); //4 bytes

  comp->assemblyLine += 6;
  codeList_insertCodes(comp->codes,JUMP_GREATER); //2 bytes
  comp->jumpCodes = jumpList_insertCodeNode(comp->jumpCodes,codeList_insertJumpCode(comp->codes,n3),comp->assemblyLine); //4 bytes
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

const char *getOp(Compiler *comp,char op){
  // POR ALGUM MOTIVO O DICIONÁRIO NÃO FUNCIONOU, ENTÃO RECORRI AO IF ELSE MESMO, EM TERNÁRIO
  // char opp[2] = {op,'\0'};
  // void *v = dict_getValue(comp->opCodes,opp);
  // char *ch = *((char **)dict_getValue(comp->opCodes,opp));
  // return ch;
  return (op=='+') ? SUM : (op=='-') ? SUB : MUL;
}

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(-1);
}