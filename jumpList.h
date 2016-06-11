#include "codeList.h"
#include "dict.h"

typedef struct jmpNode JumpNode;

//Creates a new list
JumpNode *jumpList_init();

//Inserts a char code in the list
JumpNode *jumpList_insertCodeNode(JumpNode *jn, CodeNode *cn);

void jumpList_prepJumps(JumpNode *jn, Dict *linesDict);

void jumpList_free(JumpNode *jn);