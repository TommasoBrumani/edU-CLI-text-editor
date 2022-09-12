/* This version uses pointers instead of global variables */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#define MAX 1024+1
#define INCREASE 100
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

void menu ();
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

/*
void readUndo ();
void readRedo ();
void readStuff ();
*/

int main (int argc, char*argv[])
{   
    char**docHead=NULL;
    row_t*undoHead=NULL;
    row_t*redoHead=NULL;
    int size, lastRow, i;
    char*in;

    size=0;
    lastRow=0;

    in=input ();
    
    for (i=0; *(in+i)!='\0' && in[i]!='q';) {
        if (in[i]=='c'){
            eduC(in, &docHead, &undoHead, &redoHead, &size, &lastRow);
            in=input ();
            i=0;
        }else if (in[i]=='d'){
            eduD(in, &docHead, &undoHead, &redoHead, &size, &lastRow);
            in=input ();
            i=0;
        }else if (in[i]=='p'){
            eduP(in, &docHead, &lastRow);
            in=input ();
            i=0;
        }else if (in[i]=='u'){
            eduU(in, &docHead, &undoHead, &redoHead, &lastRow);
            in=input ();
            i=0;
        }else if (in[i]=='r'){
            eduR(in, &docHead, &undoHead, &redoHead, &lastRow);
            in=input ();
            i=0;
        }else
            i++;

    }

    /*printf ("See you, Space Cowboy.\n");*/
    
    return 0;
}

void menu ()
{
    printf ("Hello Space Cowboy, welcome to the editor.\n");
    printf ("Every command can be followed by one or more rows of text,\nand are terminated by a row containing only the '.' character.\n");
    printf ("Here are the accepted commands:\n");
    printf ("1. ind1,ind2c: Substitute chosen rows with provided rows of text\n2. ind1,ind2d: Delete chosen rows\n3. ind1,ind2p: Print chosen rows\n4. (number)u: Undo the last (number) 'c' or 'd' commands\n5. (number)r: Redo the last (number) undone commands\n6. q: Exit the editor\n");
    
    return;
}

char* input ()
{
    char tmp [MAX];
    char*in;
    size_t len;
    int size;

    /*menu();*/

    /*printf ("Insert your input:\n");*/

    fgets (tmp, MAX, stdin);
    len=strlen(tmp);
    size=len+1;
    if ((in=malloc(sizeof(char)*(size)))!=NULL) {
        
        in=strcpy(in, tmp);
    } else
        printf ("makeString: memory allocation error.\n");

    return in;
}

void readIndex (char in[], int*ind1, int*ind2)
{
    char num[MAX];
    int i, j;

    for (i=0; in[i]!=','; i++) {
        num[i]=in[i];
    }
    num[i]='\0';
    j=atoi(num);
    *ind1=j-1;

    /*printf ("%d\n", *ind1+1);*/

    for (i++, j=0; in[i]!='c' && in[i]!='d' && in[i]!='p'; i++, j++) {
        num[j]=in[i];
    }
    num[j]='\0';
    j=atoi(num);
    *ind2=j-1;

    /*printf ("%d\n", *ind2+1);*/

    return;
}

int readNumber (char in[])
{
    char num[MAX];
    int i;

    for (i=0; in[i]!='u' && in[i]!='r'; i++) {
        num[i]=in[i];
    }
    num[i]='\0';

    return (atoi(num));
}

void reSize (char***docHead, int*size, int*lastRow)
{   
    char**tmp;
    int i, num, lr;

    lr=*lastRow;
    tmp=*docHead;
    num=*size+INCREASE;
    if ((*docHead=malloc(sizeof(char*)*(num)))!=NULL) {
        
        /*printf ("Resizing to size %d...\n", num);*/

        for (i=0; i<lr; i++) {
            *((*docHead)+i)=tmp[i];
        }
        free(tmp);
        *size=num;
        return;
    } else
        printf ("reSize: memory allocation error.\n");
    *docHead=tmp;

    return;
}

void eduC (char in[], char***docHead, row_t**undoHead, row_t**redoHead, int*size, int*lastRow)
{
    int ind1, ind2;
    int i;
    char fullStop [FSSIZE];

    readIndex(in, &ind1, &ind2);
    free(in);

    while (*size <= ind2) {
        reSize (docHead, size, lastRow);
    }

    copyUndo (ind1, ind2, docHead, undoHead, lastRow);

    for (i=ind1; i<=ind2; i++) {
        *((*docHead)+i)=input();
    }

    fgets (fullStop, FSSIZE, stdin);

    emptyRedo(redoHead);

    return;
}

void eduD (char in[], char***docHead, row_t**undoHead, row_t**redoHead, int*size, int*lastRow)
{
    int ind1, ind2;

    readIndex(in, &ind1, &ind2);
    free(in);

    while (*size <= ind2) {
        reSize (docHead, size, lastRow);
    }
    
    deleteUndo (ind1, ind2, docHead, undoHead, lastRow);

    emptyRedo(redoHead);

    return;
}

void eduP (char in[], char***docHead, int*lastRow)
{
    int ind1, ind2, lr;
    char**doc;

    readIndex(in, &ind1, &ind2);
    free(in);

    for (; ind1<0; ind1++) {
        printf (".\n");
    }
    for (doc=*docHead, lr=*lastRow; ind1<=ind2 && ind1<lr; ind1++) {
        printf ("%s", doc[ind1]);
    }
    for (; ind1<=ind2; ind1++) {
        printf (".\n");
    }

    return;
}

void eduU (char in[], char***docHead, row_t**undoHead, row_t**redoHead, int*lastRow)
{
    int n;
    int i, diff;
    int ind1, ind2, isC;
    char**row;

    n=readNumber(in);
    free(in);

    for (; *undoHead!=NULL && n>0; n--) {
        
        ind1=(*undoHead)->ind1;
        ind2=(*undoHead)->ind2;
        isC=(*undoHead)->isC;
        row=(*undoHead)->row;
        diff=ind2-ind1+1;

        if (isC==COPY) {

            for (i=0; row[i]!=NULL; i++);

            pushStack (ind1, ind2, isC, docHead, redoHead, lastRow);

            if (i<diff)
                *lastRow=i+ind1;

        } else {
            if (*lastRow<=ind1) {

                for (i=0; i<diff && row[i]!=NULL; i++);

                pushStack (ind1, ind2, isC, docHead, redoHead, lastRow);

                *lastRow=i+ind1;

            } else {

                for (i=*lastRow-1; i>=ind1; i--) 
                    *((*docHead)+i+diff)=*((*docHead)+i);

                i=*lastRow+diff;
                *lastRow=ind1;

                pushStack (ind1, ind2, isC, docHead, redoHead, lastRow);

                *lastRow=i;
            }
        }  

        popStack (docHead, undoHead, lastRow); 
    }

    return;
}

void eduR (char in[], char***docHead, row_t**undoHead, row_t**redoHead, int*lastRow)
{   
    int n;
    int ind1, ind2;

    n=readNumber(in);
    free(in);

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

    pushStack (ind1, ind2, COPY, docHead, undoHead, lastRow);

    if (ind2>=*lastRow) {
        *lastRow=ind2+1; 
    }

    return;
}

void deleteUndo (int ind1, int ind2, char***docHead, row_t**undoHead, int*lastRow) 
{
    int i, top, lr;

    pushStack (ind1, ind2, DELETE, docHead, undoHead, lastRow);

    if (ind2>=*lastRow) {

        if (ind1<=*lastRow) 
            *lastRow=ind1;

    } else {

        for (i=ind1, top=ind2+1, lr=*lastRow; top<lr; i++, top++)
            *((*docHead)+i)=*((*docHead)+top);
        *lastRow=i;
    }

    return;
}

row_t* makeRow (int ind1, int ind2, char***docHead, int*lastRow)
{   
    row_t*new;
    int i, j, top;
    char**row;  

    /* top variable indicates first position with empty row */

    if ((new=malloc(sizeof(row_t)))!=NULL) {

        if (ind2>=*lastRow)
            top=*lastRow;
        else
            top=ind2+1;

        if ((new->row=malloc(sizeof(char*)*(top-ind1+1)))!=NULL) { 
            row=new->row;
            for (i=ind1, j=0; i<top; i++, j++)
                row[j]=*((*docHead)+i);
            row[j]=NULL;

        } else 
            printf ("makeRow: memory allocation error for row array.\n");

        new->ind1=ind1;
        new->ind2=ind2;
    } else
        printf ("makeRow: memory allocation error for row_t struct.\n");

    return new;
}

void pushStack (int ind1, int ind2, _Bool isC, char***docHead, row_t**stackHead, int*lastRow)  
{
    row_t*new;

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

    ind1=(*stackHead)->ind1;
    ind2=(*stackHead)->ind2;
    row=(*stackHead)->row;
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
    row_t*del;
    char**row;
    int i;

    while (*redoHead!=NULL) {
        del=*redoHead;
        row=del->row;
        for (i=0; row[i]!=NULL; i++)
            free(row[i]);
        free(row);
        *redoHead=(*redoHead)->next;
        free(del);
    }

    return;
}

/*
void readUndo ()
{
    row_t*curr;
    int i, j;

    printf ("HERE ARE THE UNDO PILE CONTENTS:\n");
    for (curr=undoHead, i=1;  curr!=NULL; curr=curr->next, i++) {
        printf ("Undo element number %d has:\nRows:\n", i);
        for (j=0; curr->row[j]!=NULL; j++)
            printf ("Row %d: %s\n", j+1, curr->row[j]);
        printf ("IND1: %d\nIND2: %d\nisC: %d\n", curr->ind1, curr->ind2, curr->isC);
    }

    return;
}

void readRedo ()
{
    row_t*curr;
    int i, j;

    printf ("HERE ARE THE REDO PILE CONTENTS:\n");
    for (curr=redoHead, i=1; curr!=NULL; curr=curr->next, i++) {
        printf ("Redo element number %d has:\nRows:\n", i);
        for (j=0; curr->row[j]!=NULL; j++)
            printf ("Row %d: %s\n", j+1, curr->row[j]);
        printf ("IND1: %d\nIND2: %d\nisC: %d\n", curr->ind1, curr->ind2, curr->isC);
    }

    return;
}

void readStuff () {
    printf ("LastRow is: %d\nSize is: %d\n", lastRow, size);
    return;
}
*/