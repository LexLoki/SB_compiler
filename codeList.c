#include "codeList.h"
#include <stdlib.h>

struct node{
	char code;
	struct node *next;
};

struct codeList{
	struct node *first, *last;
	int n;
};

CodeList *codeList_init(){
	CodeList *new = (CodeList*)malloc(sizeof(CodeList));
	if(new==NULL) exit(-1);
	new->first = new->last = NULL;
	new->n = 0;
	return new;
}

struct node *newNode(char code){
	struct node *new = (struct node*)malloc(sizeof(struct node));
	if(new==NULL) exit(-1);
	new->code = code;
	return new;
}

void codeList_insertCode(CodeList *cd, char code){
	struct node *new = newNode(code);
	new->next = NULL;
	if(cd->first == NULL)
		cd->first = new;
	else
		cd->last->next = new;
	cd->last = new;
	cd->n++;
}

CodeNode *codeList_insertJumpCode(CodeList *cd, int num){
	int aux,i;
	CodeNode *ret;
	aux = num/16;
	codeList_insertCode(cd,num%16+(aux%16)*16);
	num = aux/16;
	ret = cd->last;
	for(i=0;i<3;i++){
		aux = num/16;
		codeList_insertCode(cd,num%16+(aux%16)*16);
		num = aux/16;
	}
	return ret;
}

void codeList_insertCodes(CodeList *cd, const char *codes){
	int i;
	for(i=0;codes[i]!='\0';codeList_insertCode(cd,codes[i++]));
}

void codeList_insertInt(CodeList *cd, int num){
	int aux,i;
	for(i=0;i<4;i++){
		aux = num/16;
		codeList_insertCode(cd,num%16+(aux%16)*16);
		num = aux/16;
	}
}

void codeList_insertPrepCode(CodeList *cd, char code){
	struct node *new = newNode(code);
	new->next = cd->first;
	cd->first = new;
	if (cd->last == NULL) cd->last = new;
	cd->n++;
}

void codeList_insertPrepCodes(CodeList *cd, const char *codes){
	if(codes[0]!='\0'){
		codeList_insertPrepCodes(cd,codes+1);
		codeList_insertPrepCode(cd,codes[0]);
	}
}

int codeList_isEmpty(CodeList *cd){
	return (cd->n==0);
}

int codeList_getSize(CodeList *cd){
	return cd->n;
}

void codeNode_changeInt(CodeNode *cn, int num){
	int i,aux;
	for(i=0;i<4;i++,cn=cn->next){
		aux = num/16;
		cn->code = num%16+(aux%16)*16;
		num = aux/16;
	}
}

int codeNode_getInt(CodeNode *cn){
	CodeNode *aux = cn;
	int num=0, mult=1,i;
	for(i=0;i<4;i++,aux=aux->next){
		num += aux->code*mult;
		mult *= 16*16;
	}
	return num;
}

char *codeList_toArray(CodeList *cd){
	int i;
	struct node *aux=cd->first;
	char *arr = (char*)malloc(sizeof(char)*(cd->n));
	if(arr==NULL) exit(-1);
	for(i=0;i<cd->n;arr[i++]=aux->code,aux=aux->next);
	//arr[i]='\0';
	return arr;
}

void codeList_fillArray(CodeList *cd, char *arr){
	int i;
	struct node *aux=cd->first;
	for(i=0;i<cd->n;arr[i++]=aux->code,aux=aux->next);
}

void codeList_free(CodeList *cd){
	struct node *aux;
	for(;cd->first!=NULL;aux=cd->first,cd->first=aux->next,free(aux));
	free(cd);
}