typedef struct codeList CodeList;

//Creates a new list
CodeList *codeList_init();

//Inserts a char code in the list
void codeList_insertCode(CodeList *cd, char code);

//Inserts a array of char codes in the list, ending with '\0'
void codeList_insertCodes(CodeList *cd, const char *codes);

//Inserts a integer in the list, useful for constants
void codeList_insertInt(CodeList *cd, int num);

//Inserts a char code in the beginning of the list, useful for the stack preparation code
void codeList_insertPrepCode(CodeList *cd, char code);

void codeList_insertPrepCodes(CodeList *cd, const char *codes);

int codeList_getSize(CodeList *cd);

int codeList_isEmpty(CodeList *cd); //Might not get to be used

//Allocates and return a array of codes with the codes in the list
char *codeList_toArray(CodeList *cd);

//Free the list
void codeList_free(CodeList *cd);