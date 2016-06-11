#include "jumpList.h"
#include <stdlib.h>

struct jmpNode{
	CodeNode *codeNode;
	struct jmpNode *next;
};

JumpNode *jumpList_init(){
	return (JumpNode*)NULL;
}

void jumpList_insertCodeNode(JumpNode *jn, CodeNode *cn){
	JumpNode *new = (JumpNode*)malloc(sizeof(JumpNode));
	if(new==NULL) exit(-1);
	new->codeNode = cn;
	new->next = jn;
	jn = new;
}

void jumpList_prepJumps(JumpNode *jn, Dict *linesDict){
	JumpNode *aux;
	char key[2] = {0,0};
	for(aux=jn;aux!=NULL;aux=aux->next){
		key[0] = codeNode_get(aux->codeNode);
		codeNode_change(aux->codeNode,*((int*)dict_getValue(linesDict,key)));
	}

}

void jumpList_free(JumpNode *jn){
	JumpNode *aux;
	for(;jn!=NULL;aux=jn,jn=aux->next,free(aux));
}