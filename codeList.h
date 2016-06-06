typedef struct codeList CodeList;

CodeList *codeList_init();

void codeList_insertBegin();

void codeList_insertCode(CodeList *cd, char code);

void codeList_insertPrepCode(CodeList *cd, char code);

void codeList_isEmpty(CodeList *cd); //Might not get to be used

char *codeList_toArray(CodeList *cd);

void codeList_free(CodeList *cd);