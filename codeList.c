#include "list.h"

struct node{
	char code;
	struct list *next;
}

struct codeList{
	struct node *first, *last;
	int n;
}

CodeList *codeList_init(){
	CodeList *new = (CodeList*)malloc(sizeof(CodeList));
	if(new==NULL) exit(-1);
	new->first = new->last = NULL;
	new-n = 0;
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

void codeList_insertPrepCode(CodeList *cd, char code){
	struct node *new = newNode(code);
	new->next = cd->first;
	cd->first = new;
	if (cd->last == NULL) cd->last = new;
}

void codeList_isEmpty(CodeList *cd){
	return (cd->n==0);
}

char *codeList_toArray(CodeList *cd){
	int i;
	struct node *aux=cd->first;
	char *arr = (char*)malloc(sizeof(char)*(cd->n+1));
	if(arr==NULL) exit(-1);
	for(i=0;i<cd->n;arr[i++]=aux->code;aux=aux->next);
	arr[i]='\0';
	return arr;
}

void codeList_free(CodeList *cd){
	struct node *aux;
	for(;cd->first!=NULL;aux=cd->first;cd->first=aux->next;free(aux));
	free(cd);
}