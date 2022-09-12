/* This version uses pointers instead of global variables, puts_unlocked function for output, and fgets_unlocked for input */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#define MAX 1024+1
#define INCREASE 1000
#define UNDO 1
#define REDO 0
#define COPY 1
#define DELETE 0
#define FSSIZE 5

typedef struct row_s {
    char**row;
    int ind1;
    int ind2;
    _Bool isC;
    struct row_s*next;
} row_t;

char* input ();
void readIndex (char*, int*, int*);
int readNumber (char*);
void reSize (char***, int*, int*); 
void eduC (char*, char***, row_t**, row_t**, int*, int*);
void eduD (char*, char***, row_t**, row_t**, int*, int*);
void eduP (char*, char***, int*);
void eduU (char*, char***, row_t**, row_t**, int*);
void eduR (char*, char***, row_t**, row_t**, int*);
void copyUndo (int, int, char***, row_t**, int*);
void deleteUndo (int, int, char***, row_t**, int*);
row_t* makeRow (int, int,  char***, int*);
void pushStack (int, int, _Bool, char***, row_t**, int*);
void popStack (char***, row_t**, int*);
void emptyRedo (row_t**);


int main (int argc, char*argv[])
{   
    char**docHead=NULL; /* Head of doc array, every element is a row */
    row_t*undoHead=NULL; /* Head of undo pile */
    row_t*redoHead=NULL; /* Head of redo pile */
    int size, lastRow, i; /* Size is size of docHead array, lastRow is index of first empty row in doc */
    char*in; /* Command input */

    size=0;
    lastRow=0;

    in=input (); /* Reading of first input command */

    /* For loop reads input and determines what type of command was issued */
    /* After executing the command it resets the loop and asks for new input (loop ends on 'q' command) */
    
    for (i=1; in[i]!='\0' && in[i]!='q';) {
        if (in[i]=='c'){
            eduC(in, &docHead, &undoHead, &redoHead, &size, &lastRow);
            in=input ();
            i=1;
        }else if (in[i]=='d'){
            eduD(in, &docHead, &undoHead, &redoHead, &size, &lastRow);
            in=input ();
            i=1;
        }else if (in[i]=='p'){
            eduP(in, &docHead, &lastRow);
            in=input ();
            i=1;
        }else if (in[i]=='u'){
            eduU(in, &docHead, &undoHead, &redoHead, &lastRow);
            in=input ();
            i=1;
        }else if (in[i]=='r'){
            eduR(in, &docHead, &undoHead, &redoHead, &lastRow);
            in=input ();
            i=1;
        }else
            i++;

    }
    
    return 0;
}

char* input ()
{
    char tmp [MAX];
    char*in;
    size_t len;
    int size;

    /* Reads input on 1024 (max lenght) array, than copies it onto a smaller (minimum sized) one */

    fgets (tmp, MAX, stdin);
    len=strlen(tmp);
    size=len+1;

    in=malloc(sizeof(char)*(size));

    in=strcpy(in, tmp);

    return in;
}

void readIndex (char in[], int*ind1, int*ind2)
{
    int i, j, start;

    /* Each loop reads input string and extracts first and second indexes */

    for (i=0; in[i]!=','; i++);
    in[i]='\0';
    j=atoi(in);
    *ind1=j-1;

    for (i++, start=i; in[i]!='c' && in[i]!='d' && in[i]!='p'; i++);
    in[i]='\0';
    j=atoi(in+start);
    *ind2=j-1;

    return;
}

int readNumber (char in[])
{
    char num[MAX];
    int i;

    /* Reads input string and extract index number */

    for (i=0; in[i]!='u' && in[i]!='r'; i++) ;
    in[i]='\0';

    return (atoi(in));
}

void reSize (char***docHead, int*size, int*lastRow)
{   
    char**tmp;
    int i, num, lr;

    /* Creates new docHead array of size equals to previous one + INCREASE amount, and transfers rows between the two */

    lr=*lastRow;
    tmp=*docHead;

    num=*size+INCREASE;
    *docHead=malloc(sizeof(char*)*(num));
        
    for (i=0; i<lr; i++) {
        *((*docHead)+i)=tmp[i];
    }
    free(tmp);
    *size=num;

    return;
}

void eduC (char in[], char***docHead, row_t**undoHead, row_t**redoHead, int*size, int*lastRow)
{
    int ind1, ind2;
    int i;
    char fullStop [FSSIZE];

    /* Calls function to extract indexes from command input */

    readIndex(in, &ind1, &ind2);
    free(in);

    /* If the command accesses and index outside of current size, increase size */

    while (*size <= ind2) {
        reSize (docHead, size, lastRow);
    }

    /* Calls function to transfer rows included between indexes to the undo pile, because overwritten */

    copyUndo (ind1, ind2, docHead, undoHead, lastRow);


    /* Reads new strings to write on rows between indexes, then asks for '.' */

    for (i=ind1; i<=ind2; i++) {
        *((*docHead)+i)=input();
    }

    fgets (fullStop, FSSIZE, stdin);

    /* Empties redo pile */

    emptyRedo(redoHead);

    return;
}

void eduD (char in[], char***docHead, row_t**undoHead, row_t**redoHead, int*size, int*lastRow)
{
    int ind1, ind2;

    readIndex(in, &ind1, &ind2);
    free(in);

    /* If the command accesses and index outside of current size, increase size */

    while (*size <= ind2) {
        reSize (docHead, size, lastRow);
    }
    
    /* Calls function to transfer rows included between indexes to the undo pile, because deleted */

    deleteUndo (ind1, ind2, docHead, undoHead, lastRow);

    /* Empties redo pile */

    emptyRedo(redoHead);

    return;
}

void eduP (char in[], char***docHead, int*lastRow)
{
    int ind1, ind2, lr;
    char**docH;
    char fullStop [3]= ".\n\0";

    /* Calls function to extract indexes from command input */

    readIndex(in, &ind1, &ind2);
    free(in);

    /* Prints rows between indexes (non existent rows print the '.' character) */

    for (; ind1<0; ind1++) {
        fputs_unlocked(fullStop, stdout);
    }
    for (docH=*docHead, lr=*lastRow; ind1<=ind2 && ind1<lr; ind1++) {
        fputs_unlocked(docH[ind1], stdout);
    }
    for (; ind1<=ind2; ind1++) {
        fputs_unlocked(fullStop, stdout);
    }

    return;
}

void eduU (char in[], char***docHead, row_t**undoHead, row_t**redoHead, int*lastRow)
{
    int n;
    int i, diff;
    int ind1, ind2, isC;
    char**row, **docH;
    row_t*undH;

    /* Calls function to extract index from command input */

    n=readNumber(in);
    free(in);

    /* Executes undo 'n' times */

    for (; *undoHead!=NULL && n>0; n--) {

        /* Saves pointer variables in new variables to avoid costant address lookups */

        undH=*undoHead;
        ind1=(undH)->ind1;
        ind2=(undH)->ind2;
        isC=(undH)->isC;
        row=(undH)->row;
        diff=ind2-ind1+1;

        if (isC==COPY) {
            
            /* If last command is a 'copy', saves the lines it wrote onto redo pile and changes lastRow */

            for (i=0; row[i]!=NULL; i++);

            pushStack (ind1, ind2, isC, docHead, redoHead, lastRow);

            if (i<diff)
                *lastRow=i+ind1;

        } else {
            if (*lastRow<=ind1) {

                /* If last command is a 'delete' (NOT requiring row shifting) */
                /* Saves the interval of deletion onto redo pile and changes lastRow */

                for (i=0; i<diff && row[i]!=NULL; i++);

                pushStack (ind1, ind2, isC, docHead, redoHead, lastRow);

                *lastRow=i+ind1;

            } else {

                /* If last command is a 'delete' (requiring row shifting) */
                /* Saves the interval of deletion onto redo pile, shifts rows and changes lastRow */

                for (i=*lastRow-1, docH=*docHead; i>=ind1; i--) 
                    *(docH+i+diff)=*(docH+i);

                i=*lastRow+diff;
                *lastRow=ind1;

                pushStack (ind1, ind2, isC, docHead, redoHead, lastRow);

                *lastRow=i;
            }
        }  

        /* Restores rows contained in undo pile head onto their previous position in docHead */

        popStack (docHead, undoHead, lastRow); 
    }

    return;
}

void eduR (char in[], char***docHead, row_t**undoHead, row_t**redoHead, int*lastRow)
{   
    int n;
    int ind1, ind2;

    /* Calls function to extract index from command input */

    n=readNumber(in);
    free(in);

    /* Executes redo 'n' times, restoring undone commands */

    for (; *redoHead!=NULL && n>0; n--) {

        if ((*redoHead)->isC==COPY) 
            copyUndo((*redoHead)->ind1, (*redoHead)->ind2, docHead, undoHead, lastRow);
        else
            deleteUndo((*redoHead)->ind1, (*redoHead)->ind2, docHead, undoHead, lastRow);
            
        popStack(docHead, redoHead, lastRow);
    }

    return;
} 

void copyUndo (int ind1, int ind2, char***docHead, row_t**undoHead, int*lastRow) 
{
    int i;

    /* Calls function to store rows between indexes in undo Pile */

    pushStack (ind1, ind2, COPY, docHead, undoHead, lastRow);

    /* Changes lastRow accordingly */

    if (ind2>=*lastRow) {
        *lastRow=ind2+1; 
    }

    return;
}

void deleteUndo (int ind1, int ind2, char***docHead, row_t**undoHead, int*lastRow) 
{
    int i, top, lr;
    char**docH;

    /* Calls function to store rows between indexes in undo Pile */

    pushStack (ind1, ind2, DELETE, docHead, undoHead, lastRow);

    if (ind2>=*lastRow) {

        /* If deletion does not require row shifting, simply updates lastRow */

        if (ind1<=*lastRow) 
            *lastRow=ind1;

    } else {

        /* If deletion does require row shifting, shifts rows and then updates lastRow */

        for (i=ind1, top=ind2+1, lr=*lastRow, docH=*docHead; top<lr; i++, top++)
            *((docH)+i)=*((docH)+top);
        *lastRow=i;
    }

    return;
}

row_t* makeRow (int ind1, int ind2, char***docHead, int*lastRow)
{   
    row_t*new;
    int i, j, top;
    char**row, **docH;  

    /* Creates  new row_t element for storage in undo or redo pile */
    /* 'Top' variable indicates when to stop copying rows unto undo pile */
    /* (is either first empty row inside index interval, or first position outside interval) */

    new=malloc(sizeof(row_t));

    if (ind2>=*lastRow)
        top=*lastRow;
    else
        top=ind2+1;

    /* Stores non empty rows between indexes in 'rows' field of the new struct element, followed by a NULL */

    new->row=malloc(sizeof(char*)*(top-ind1+1));
    row=new->row;
    for (i=ind1, j=0, docH=*docHead; i<top; i++, j++)
        row[j]=*((docH)+i);
    row[j]=NULL;  

    /* Stores indexes of command into the new struct element */

    new->ind1=ind1;
    new->ind2=ind2;

    return new;
}

void pushStack (int ind1, int ind2, _Bool isC, char***docHead, row_t**stackHead, int*lastRow)  
{
    row_t*new;

    /* Calls function to make new undo or redo pile element from docHead rows between indexes */
    /* The adds onto redo or undo pile, as dictated by content of 'stackHead' variable */

    new=makeRow (ind1, ind2, docHead, lastRow);
    new->isC=isC;
    new->next=*stackHead;
    *stackHead=new;

    return;
} 

void popStack (char***docHead, row_t**stackHead, int*lastRow)
{
    row_t*newHead;
    int i;
    int ind1, ind2;
    char**row;

    /* Saves pointer variables in new variables to avoid costant address lookups */

    ind1=(*stackHead)->ind1;
    ind2=(*stackHead)->ind2;
    row=(*stackHead)->row;

    /* Copies rows contained in first element of undo or redo pile (as dictated by 'stackHead') */
    /* Onto their former positions on the document, then removes pile head */

    for (i=0; ind1<=ind2 && row[i]!=NULL; i++, ind1++) {
        *((*docHead)+ind1)=row[i];
    }
    newHead=(*stackHead)->next;
    free(row);
    free(*stackHead);
    *stackHead=newHead;

    return;
}

void emptyRedo (row_t**redoHead)
{
    row_t*del, *reH;
    char**row;
    int i;

    /* Sequentially deletes redo pile */
    
    reH=*redoHead;
    while (reH!=NULL) {
        del=reH;
        row=del->row;
        for (i=0; row[i]!=NULL; i++)
            free(row[i]);
        free(row);
        reH=(reH)->next;
        free(del);
    }
    *redoHead=reH;

    return;
}
