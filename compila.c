#include "dict.h"
#include "codeList.h"

typedef int (*funcp) ();

typedef struct compiler{
	CodeList *codes;	// Armazena os códigos de máquina resultantes em uma lista encadeada
	Dict *opCodes;		// Códigos de máquina por operação
	Dict *handlers; 	// Armazena referência às funções de tratamento (atribuicao, retorno, desvio)
	Dict *locals; 		// Armazena posições de cada variável local de acordo com seu índice
	FILE *f; 			// Ponteiro para arquivo sendo usado na compilação
	int line; 			// Identifica a linha atual
	int varQuant; 		// Contador de variáveis locais utilizadas
}Compiler;

typedef void (*opHandler)(Compiler*); // Protótipo das funções de tratamento

static void error (const char *msg, int line); 	// **
void checkVar(char var, int idx, int line); 	// Assertivas fornecidas pela professora
void checkVarP(char var, int idx, int line);	// **

int getLocal(Compiler *comp,int idx); // Fornece a posição da variável local na pilha dado o seu índice
int getOp(Compiler *comp,char op);

void varHandler(Compiler *comp);	// **
void retHandler(Compiler *comp);	// Funções de tratamento
void ifHandler(Compiler *comp);		// **
void handle(Compiler *comp, char key);

Dict *prepareHandlers();			// Prepara as referências às funções de tratamento
Dict *prepareOps();
Compiler *compiler_init(FILE *f);	// Inicializa o compilador

#define SUM 0x0 //TO DO: MACHINE CODE TO SUM %r10d TO %r11d -> addl %r10d, %r11d
#define SUB 0X0 //TO DO: MACHINE CODE TO SUB %r10d from %r11d -> ...
#define MUL 0x0 //TO DO: MACHINE CODE TO MUL %r10d TO %r11d
#define LEAVE 0x0 //TO DO: MACHINE CODE TO LEAVE
#define RET 0x0 //TO DO: MACHINE CODE TO RET
#define STACK_PUSH 0x0 //TO DO: MACHINE CODE TO: pushq %rbp
#define STACK_MOVE 0x0 //TO DO: MACHINE CODE TO: movq %rsp, %rbp


funcp compila (FILE *f){
	Compiler *comp = compiler_init(f);
	char c;
	int q;
	
	while ((c = fgetc(comp->f)) != EOF){
		handle(comp,c);
    	fscanf(comp->f, " ");
  	}
  	int q = (((comp->varQuant*4-1)/16)+1)*16;
  	//codeList_insertPrepCode(comp->codes,); //subq $q, %rsp
  	codeList_insertPrepCode(comp->codes,STACK_MOVE);
  	codeList_insertPrepCode(comp->codes,STACK_PUSH);
  	char *codes = codeList_toArray(comp->codes);
  	compiler_free(comp);
  	return codes;
}

Dict *prepareHandlers(){
	opHandler handlers[2] = {varHandler,retHandler,ifHandler};
	Dict *d = dict_init(sizeof(int*));
	dict_set(d,"v",&handlers[0]);
	dict_set(d,"r",&handlers[1]);
	dict_set(d,"i",&handlers[2]);
	return d;
}

Dict *prepareOps(){
	Dict *d = dict_init(sizeof(char));
	dict_set(d,"+",&SUM);
	dict_set(d,"-",&SUB);
	dict_set(d,"*",&MUL);
	return codes;
}

Compiler *compiler_init(FILE *f){
	Compiler *comp = (Compiler*)malloc(sizeof(Compiler));
	if(comp==NULL) exit(-1);
	comp->handlers = prepareHandlers();
	comp->codes = codeList_init();
	comp->locals = dict_init(sizeof(long));   
	comp->f = f;
	comp->varQuant = 0;
	comp->line = 0;
	return comp;
}

void compiler_free(Compiler *comp){
	codeList_free(comp->codes);
	dict_free(comp->opCodes);
	dict_free(comp->handlers);
	dict_free(comp->locals);
	free(comp);
}

void handle(Compiler *comp, char key){
	char k[2] = {key,'\0'};
	(*((opHandler*)dict_getValue(comp->handlers,&k)))(comp);
	comp->line++;
}

void retHandler(Compiler *comp){
	int idx, space;
    char var;
    if (fscanf(myfp, "et %c%d", &var, &idx) != 2)
    	error("comando invalido", line);
    if (var != '$'){
    	checkVarP(var, idx, line);
    	// CODIGO DE MAQUINA movl $idx, %eax
    }
    else{
    	space = getLocal(comp,idx);
    	// CODIGO DE MAQUINA movl -4*space(%rbp), %eax
    }
    codeList_insert(comp->codes,LEAVE);
    codeList_insert(comp->codes,RET);
    //printf("ret %c%d\n", var, idx);
}

void varHandler(Compiler *comp){
	int idx0, idx1, idx2,space;
    char var0 = c, var1, var2;
    char op;
    if (fscanf(myfp, "%d = %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2, &idx2) != 6)
    	error("comando invalido", line);
    checkVar(var0, idx0, comp->line);
    if (var1 != '$'){
    	checkVarP(var1, idx1, comp->line);
    	//codeList_insert(comp->codes,0Xa4+idx1);
    	// CODIGO DE MAQUINA movl $idx1, %r11d
    }
    else{
    	space = getLocal(comp,idx1);
    	//codeList_insert(comp->codes,0xb-4*space);
    	8
    	2 + 3 + 3
    	0xb1   48
    	0xb2   49
    	0xb3   50

    	0 0 0 0 0 0 0 0
    	    1
    	    2
    	    3 

    	// CODIGO DE MAQUINA movl -4*space(%rpb), %r11d
    }
    if (var2 != '$'){
    	checkVarP(var2, idx2, comp->line);
    	// CODIGO DE MAQUINA movl $idx2, %r10d
    }
    else{
    	space = getLocal(comp,idx2);
    	// CODIGO DE MAQUINA movl -4*space(%rpb), %r10d
    }
    codeList_insert(comp->codes,getOp(comp,op)); //Code for operation

    int space = getLocal(idx0);
    // CODIGO DE MAQUINA X = movl %r11d, -4*space(%rbp)
    // codeList_insert(comp->codes,X);
    //printf("%c%d = %c%d %c %c%d\n",var0, idx0, var1, idx1, op, var2, idx2);
}

void ifHandler(Compiler *comp){
	int idx, n1, n2, n3;
    char var;
    if (fscanf(myfp, "f %c%d %d %d %d", &var, &idx, &n1, &n2, &n3) != 5)
    error("comando invalido", line);
    if (var != '$') checkVar(var, idx, line);
    printf("if %c%d %d %d %d\n", var, idx, n1, n2, n3);
}

int getLocal(Compiler *comp,int idx){
	char key[3];
	int *val;
	sprintf(key,"%d",idx);
	val = (int*)dict_getValue(comp->locals,key);
    if(val==NULL){ //checking if this variable was used before
    	dict_set(comp->locals,key,++comp->varQuant);
    	return comp->varQuant;
    }
    return *val;
}

char getOp(Compiler *comp,char op){
	return *((char *)dict_getValue(comp->opCodes,{op,'\0'}));
}

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
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