/* This version uses global variables, but can't pass Rolling Back test */
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

char**docHead;
row_t*undoHead=NULL;
row_t*redoHead=NULL;
int size=0;
int lastRow=0;

void menu ();
char* input ();
void readIndex (char[], int*, int*);
int readNumber (char[]);
void reSize ();
void eduC (char []);
void eduD (char []);
void eduP (char []);
void eduU (char []);
void eduR (char []);
void copyUndo (int, int);
void deleteUndo (int, int);
row_t* makeRow (int, int);
void pushStack (int, int, _Bool, _Bool);
void popStack (_Bool);
void emptyRedo ();

void readUndo ();
void readRedo ();
void readStuff ();


int main (int argc, char*argv[])
{   
    char*in;
    int i;

    in=input ();
    
    for (i=0; *(in+i)!='\0' && in[i]!='q';) {
        if (in[i]=='c'){
            eduC(in);
            in=input ();
            i=0;
        }else if (in[i]=='d'){
            eduD(in);
            in=input ();
            i=0;
        }else if (in[i]=='p'){
            eduP(in);
            in=input ();
            i=0;
        }else if (in[i]=='u'){
            eduU(in);
            in=input ();
            i=0;
        }else if (in[i]=='r'){
            eduR(in);
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

void reSize ()
{   
    char**tmp;
    int i, num;

    tmp=docHead;
    num=size+INCREASE;
    if ((docHead=malloc(sizeof(char*)*(num)))!=NULL) {
        
        /*printf ("Resizing to size %d...\n", num);*/

        for (i=0; i<lastRow; i++) {
            docHead[i]=tmp[i];
        }
        free(tmp);
        size=num;
        return;
    } else
        printf ("reSize: memory allocation error.\n");
    docHead=tmp;

    return;
}

void eduC (char in[])
{
    int ind1, ind2;
    int i;
    char fullStop [FSSIZE];

    readIndex(in, &ind1, &ind2);
    free(in);

    while (size <= ind2) {
        reSize ();
    }

    copyUndo (ind1, ind2);

    for (i=ind1; i<=ind2; i++) {
        docHead[i]=input();
    }

    fgets (fullStop, FSSIZE, stdin);

    emptyRedo();

    return;
}

void eduD (char in[])
{
    int ind1, ind2;

    readIndex(in, &ind1, &ind2);
    free(in);

    while (size <= ind2) {
        reSize ();
    }
    
    deleteUndo (ind1, ind2);

    emptyRedo();

    return;
}

void eduP (char in[])
{
    int ind1, ind2;

    readIndex(in, &ind1, &ind2);
    free(in);

    for (; ind1<0; ind1++) {
        printf (".\n");
    }
    for (; ind1<=ind2 && ind1<lastRow; ind1++) {
        printf ("%s", docHead[ind1]);
    }
    for (; ind1<=ind2; ind1++) {
        printf (".\n");
    }

    return;
}

void eduU (char in[])
{
    int n;
    int i, diff;
    int ind1, ind2, isC;
    char**row;

    n=readNumber(in);
    free(in);

    for (; undoHead!=NULL && n>0; n--) {
        
        ind1=undoHead->ind1;
        ind2=undoHead->ind2;
        isC=undoHead->isC;
        row=undoHead->row;
        diff=ind2-ind1+1;

        if (isC==COPY) {

            for (i=0; row[i]!=NULL; i++);

            pushStack (ind1, ind2, isC, REDO);

            if (i<diff)
                lastRow=i+ind1;

        } else {
            if (lastRow<=ind1) {

                for (i=0; i<diff && row[i]!=NULL; i++);

                pushStack (ind1, ind2, isC, REDO);

                lastRow=i+ind1;

            } else {

                for (i=lastRow-1; i>=ind1; i--) 
                    docHead[i+diff]=docHead[i];

                i=lastRow+diff;
                lastRow=ind1;

                pushStack (ind1, ind2, isC, REDO);

                lastRow=i;
            }
        }  

        popStack (UNDO); 
    }

    i=0;

    return;
}

void eduR (char in[])
{   
    int n;
    int ind1, ind2;

    n=readNumber(in);
    free(in);

    for (; redoHead!=NULL && n>0; n--) {

        if (redoHead->isC==COPY) 
            copyUndo(redoHead->ind1, redoHead->ind2);
        else
            deleteUndo(redoHead->ind1, redoHead->ind2);
            
        popStack(REDO);
    }

    n=0;

    return;
} 

void copyUndo (int ind1, int ind2) 
{
    int i;

    pushStack (ind1, ind2, COPY, UNDO);

    if (ind2>=lastRow) {
        lastRow=ind2+1; 
    }

    return;
}

void deleteUndo (int ind1, int ind2) 
{
    int i, top;

    pushStack (ind1, ind2, DELETE, UNDO);

    if (ind2>=lastRow) {

        if (ind1<=lastRow) 
            lastRow=ind1;

    } else {

        for (i=ind1, top=ind2+1; top<lastRow; i++, top++)
            docHead[i]=docHead[top];
        lastRow=i;
    }

    return;
}

row_t* makeRow (int ind1, int ind2)
{   
    row_t*new;
    int i, j, top;
    char**row;  

    /* top variable indicates first position with empty row */

    if ((new=malloc(sizeof(row_t)))!=NULL) {

        if (ind2>=lastRow)
            top=lastRow;
        else
            top=ind2+1;

        if ((new->row=malloc(sizeof(char*)*(top-ind1+1)))!=NULL) { 
            row=new->row;
            for (i=ind1, j=0; i<top; i++, j++)
                row[j]=docHead[i];
            row[j]=NULL;

        } else 
            printf ("makeRow: memory allocation error for row array.\n");

        new->ind1=ind1;
        new->ind2=ind2;
    } else
        printf ("makeRow: memory allocation error for row_t struct.\n");

    return new;
}

void pushStack (int ind1, int ind2, _Bool isC, _Bool isUndo)  
{
    row_t*new;

    new=makeRow (ind1, ind2);
    new->isC=isC;

    if (isUndo==UNDO) {
        new->next=undoHead;
        undoHead=new;
    } else {
        new->next=redoHead;
        redoHead=new;
    }

    return;
} 

void popStack (_Bool isUndo)
{
    row_t*newHead;
    int i;
    int ind1, ind2;
    char**row;

    if (isUndo==UNDO) {
        ind1=undoHead->ind1;
        ind2=undoHead->ind2;
        row=undoHead->row;
        for (i=0; ind1<=ind2 && row[i]!=NULL; i++, ind1++) {
            docHead[ind1]=row[i];
        }
        newHead=undoHead->next;
        free(row);
        free(undoHead);
        undoHead=newHead;
    } else {
        ind1=redoHead->ind1;
        ind2=redoHead->ind2;
        row=redoHead->row;
        for (i=0; ind1<=ind2 && row[i]!=NULL; i++, ind1++) {
            docHead[ind1]=row[i];
        }
        newHead=redoHead->next;
        free(row);
        free(redoHead);
        redoHead=newHead;
    }

    return;
}

void emptyRedo ()
{
    row_t*del;
    char**row;
    int i;

    while (redoHead!=NULL) {
        del=redoHead;
        row=del->row;
        for (i=0; row[i]!=NULL; i++)
            free(row[i]);
        free(row);
        redoHead=redoHead->next;
        free(del);
    }

    return;
}

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
