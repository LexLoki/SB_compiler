#include "jumpList.h"
#include <stdlib.h>
#include <stdio.h>

struct jmpNode{
	CodeNode *codeNode;
	struct jmpNode *next;
};

JumpNode *jumpList_init(){
	return (JumpNode*)NULL;
}

JumpNode *jumpList_insertCodeNode(JumpNode *jn, CodeNode *cn){
	printf("new jump %c\n",codeNode_getInt(cn));
	JumpNode *new = (JumpNode*)malloc(sizeof(JumpNode));
	if(new==NULL) exit(-1);
	new->codeNode = cn;
	new->next = jn;
	return new;
}

void jumpList_prepJumps(JumpNode *jn, Dict *linesDict, int offset){
	JumpNode *aux;
	printf("letsJump\n");
	char key[3];
	int a;
	for(aux=jn;aux!=NULL;aux=aux->next){
		printf("correcting jump\n");
		//key[0] = codeNode_get(aux->codeNode);
		a = codeNode_getInt(aux->codeNode);
		printf("%d\n",a);
		sprintf(key,"%d",a);
		codeNode_changeInt(aux->codeNode,offset+*((int*)dict_getValue(linesDict,key)));
	}

}

void jumpList_free(JumpNode *jn){
	JumpNode *aux;
	for(;jn!=NULL;aux=jn,jn=aux->next,free(aux));
}