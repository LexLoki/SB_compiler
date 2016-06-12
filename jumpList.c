#include "jumpList.h"
#include <stdlib.h>
#include <stdio.h>

struct jmpNode{
	CodeNode *codeNode;
	int fromCode;
	struct jmpNode *next;
};

JumpNode *jumpList_init(){
	return (JumpNode*)NULL;
}

JumpNode *jumpList_insertCodeNode(JumpNode *jn, CodeNode *cn, int fromCode){
	printf("new jump %c\n",codeNode_getInt(cn));
	JumpNode *new = (JumpNode*)malloc(sizeof(JumpNode));
	if(new==NULL) exit(-1);
	new->codeNode = cn;
	new->fromCode = fromCode;
	new->next = jn;
	return new;
}

void jumpList_prepJumps(JumpNode *jn, Dict *linesDict){
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
		codeNode_changeInt(aux->codeNode,*((int*)dict_getValue(linesDict,key)) - aux->fromCode);
	}

}

void jumpList_free(JumpNode *jn){
	JumpNode *aux;
	for(;jn!=NULL;aux=jn,jn=aux->next,free(aux));
}