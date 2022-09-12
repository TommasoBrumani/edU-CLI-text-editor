/* This version uses pointers instead of global variables, puts_unlocked function for output, and redo/undo optimization (current working copy) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#define MAX 1024+1
#define INCREASE 1000000
#define COPY 1
#define DELETE 0
#define FSSIZE 5
#define SNAPFREQ 10
#define ASCIIZERO 48
#define ASCIININE 57
#define ASCIIC 99
#define ASCIIU 117

/* Struct for undo and redo pile nodes */
typedef struct row_s {
    char**row;
    int ind1;
    int ind2;
    _Bool type;
    struct row_s*next;
} row_t;


/* Command execution functions */
void doCopy (char***, row_t**, int*, int*, int, int, row_t**, int*, int*, int*);
void doDelete (char**, row_t**, int*, int, int, row_t**, int*, int*, int*);
void doPrint (char**, int, int, int);
void doUndo (int*, int*, int*, int);
void doRedo (int*, int*, int*, int);
void doBalance (char**, int*, row_t**, row_t**, int, row_t**, int*, int*, int*);

/* Command execution subfunctions */
void actualUndo (char**, int*, row_t**, row_t**, int, row_t**, int*, int*, int*);
void actualRedo (char**, int*, row_t**, row_t**, int, row_t**, int*, int*, int*);

/* Structure manipulation functions */
void reSize (char***, int*); 
void pushStack (int, int, _Bool, char**, row_t**, int*, row_t**, int*);
void popStack (char**, row_t**, int*, row_t**, int*);
void pushSnap (char**, row_t**, int);
void emptyRedo (row_t**, row_t**);


/* MAIN */

int main ()
{
    char**docHead=NULL;         /* Head of document array, every element is a row */
    row_t*undoHead=NULL;        /* Head of undo pile */
    row_t*redoHead=NULL;        /* Head of redo pile */
    row_t*leftovers=NULL;       /* Head of pile of leftover row_t structures */
    int size, undoSize, redoSize, leftoverSize;   /* Size of docHead array, of undo and redo piles (counting unimplemented balance), and leftover pile */
    int firstEmpty;                     /* First empty row in docHead */ 
    int n;                              /* Counter for yet unimplemented undo/redo balance */
    int segCountU, segCountR;
    int i, j, ind1, ind2; 
    char in [MAX], command, c;             /* Command input */

    size=0;
    undoSize=0;
    redoSize=0;
    leftoverSize=0;
    firstEmpty=0;
    n=0;
    segCountU=0;
    segCountR=0;

    do {

        command='0';
        i=-1;
        j=0;

        do {

            i++;
            c=getchar();

            if (c>=ASCIIZERO && c<=ASCIININE) {
                in[i]=c;
            } else if (c==',') {
                j=i+1;
                in[i]='\0';
                ind1=atoi(in);
                ind1--;
            }else if (c>=ASCIIC && c<=ASCIIU) {
                command=c;
                in[i]='\0';
                ind2=atoi(in+j);
                ind2--;
            } 

        } while (c!='\n');

        if (command=='c') {

            doBalance (docHead, &firstEmpty ,&undoHead, &redoHead, n, &leftovers, &leftoverSize, &segCountU, &segCountR);
            n=0;
            doCopy (&docHead, &undoHead, &size, &firstEmpty, ind1, ind2, &leftovers, &leftoverSize, &segCountU, &segCountR);
            undoSize++;
            emptyRedo(&redoHead, &leftovers);
            leftoverSize+=redoSize;
            redoSize=0;

        } else if (command=='d') {

            doBalance (docHead, &firstEmpty ,&undoHead, &redoHead, n, &leftovers, &leftoverSize, &segCountU, &segCountR);
            n=0;
            doDelete (docHead, &undoHead, &firstEmpty, ind1, ind2, &leftovers, &leftoverSize, &segCountU, &segCountR);
            undoSize++;
            emptyRedo(&redoHead, &leftovers);
            leftoverSize+=redoSize;
            redoSize=0;

        } else if (command=='p') {

            doBalance (docHead, &firstEmpty ,&undoHead, &redoHead, n, &leftovers, &leftoverSize, &segCountU, &segCountR);
            n=0;
            doPrint(docHead, firstEmpty, ind1, ind2);

        } else if (command=='u') {

            ind2++;
            doUndo(&n, &undoSize, &redoSize, ind2);

        } else if (command=='r') {

            ind2++;
            doRedo(&n, &undoSize, &redoSize, ind2);

        }


    } while (command!='q');

    return 0;
}


/* Command execution functions bodies */

void doCopy (char***docHead, row_t**undoHead, int*size, int*firstEmpty, int ind1, int ind2, row_t**leftovers, int*leftoverSize, int*segCountU, int*segCountR)
{
    /* This function serves as a hub for the execution of change commands */

    int len, docS;
    char fullStop [FSSIZE];
    char *tmp, **docH;

    /* Calls function to transfer rows included between indexes to the undo pile, because overwritten */

    pushStack (ind1, ind2, COPY, *docHead, undoHead, firstEmpty, leftovers, leftoverSize);

    /* Changes firstEmpty accordingly */

    if (ind2>=*firstEmpty) {
        *firstEmpty=ind2+1; 
    }

    for (docH=*docHead, docS=*size; ind1<=ind2; ind1++) {

        if (docS <= ind1) {
            reSize (docHead, size);
            docS=*size;
            docH=*docHead;
        }
        
        tmp=malloc(sizeof(char)*(MAX));
        if (fgets (tmp, MAX, stdin)!=NULL);
        tmp=realloc(tmp, (strlen(tmp)+1));

        docH[ind1]=tmp; 

    }

    if (fgets (fullStop, FSSIZE, stdin)!=NULL);

    return;
}

void doDelete (char**docHead, row_t**undoHead, int*firstEmpty, int ind1, int ind2, row_t**leftovers, int*leftoverSize, int*segCountU, int*segCountR)
{
    /* This function saves onto the undo pile any rows about to be deleted by a delete command */

    int i, top, firstE;

    /* Calls function to store rows between indexes in undo Pile */

    pushStack (ind1, ind2, DELETE, docHead, undoHead, firstEmpty, leftovers, leftoverSize);

    if (ind2>=*firstEmpty) {

        /* If deletion does not require row shifting, simply updates firstEmpty */

        if (ind1<=*firstEmpty) 
            *firstEmpty=ind1;

    } else {

        /* If deletion does require row shifting, shifts rows and then updates firstEmpty */

        for (i=ind1, top=ind2+1, firstE=*firstEmpty; top<firstE; i++, top++)
            docHead[i]=docHead[top];
        *firstEmpty=i;
    }

    return;
}

void doPrint (char**docHead, int firstEmpty, int ind1, int ind2)
{   
    /* This function serves as a hub for the execution of print commands */

    char fullStop [3]= ".\n\0";

    /* Prints rows between indexes (non existent rows print the '.' character) */

    for (; ind1<0; ind1++) {
        fputs (fullStop, stdout);
    }
    for (; ind1<=ind2 && ind1<firstEmpty; ind1++) {
        fputs (docHead[ind1], stdout);
    }
    for (; ind1<=ind2; ind1++) {
        fputs (fullStop, stdout);
    }

    return;
}

void doUndo (int*n, int*undoSize, int*redoSize, int undos)
{
    /* This function serves as a hub for the execution of undo commands */
    
    if(undos<=*undoSize) {
        *n=*n+undos;
        *undoSize=*undoSize-undos;
        *redoSize=*redoSize+undos;
    } else {
        *n=*n+*undoSize;
        *redoSize=*redoSize+*undoSize;
        *undoSize=0;
    }
  
    return;
}

void doRedo (int*n, int*undoSize, int*redoSize, int redos)
{   
    /* This function serves as a hub for the execution of redo commands */

    if(redos<=*redoSize) {
        *n=*n-redos;
        *undoSize=*undoSize+redos;
        *redoSize=*redoSize-redos;
    } else {
        *n=*n-*redoSize;
        *undoSize=*undoSize+*redoSize;
        *redoSize=0;
    }

    return;
} 

void doBalance (char**docHead, int*firstEmpty, row_t**undoHead, row_t**redoHead, int n, row_t**leftovers, int*leftoverSize, int*segCountU, int*segCountR)
{
    if (n>0) {

        actualUndo (docHead, firstEmpty ,undoHead, redoHead, n, leftovers, leftoverSize, segCountU, segCountR);

    } else if (n<0) {

        actualRedo (docHead, firstEmpty, undoHead, redoHead, n, leftovers, leftoverSize, segCountU, segCountR);

    }

    return;
}


/* Command execution subfunctions bodies */

void actualUndo (char**docHead, int*firstEmpty, row_t**undoHead, row_t**redoHead, int n, row_t**leftovers, int*leftoverSize, int*segCountU, int*segCountR)
{
    /* This function puts into practice the correct number of undos after accounting for undo-redo subtractions */

    int i, diff, count;
    int ind1, ind2, type;
    char**row;
    row_t*undH;

    /* Executes undo 'n' times */

    for (; n>0; n--) {

        /* Saves pointer variables in new variables to avoid costant address lookups */

        undH=*undoHead;
        ind1=(undH)->ind1;
        ind2=(undH)->ind2;
        type=(undH)->type;
        row=(undH)->row;
        diff=ind2-ind1+1;

        if (type==COPY) {
            
            /* If last command is a 'copy', saves the lines it wrote onto redo pile and changes firstEmpty */

            pushStack (ind1, ind2, type, docHead, redoHead, firstEmpty, leftovers, leftoverSize);

            popStack (docHead, undoHead, &count, leftovers, leftoverSize); 

            if (count<diff)
                *firstEmpty=count+ind1;

        } else {
            if (*firstEmpty<=ind1) {

                /* If last command is a 'delete' (NOT requiring row shifting) */
                /* Saves the interval of deletion onto redo pile and changes firstEmpty */

                pushStack (ind1, ind2, type, docHead, redoHead, firstEmpty, leftovers, leftoverSize);

                popStack (docHead, undoHead, &count, leftovers, leftoverSize);

                *firstEmpty=count+ind1;

            } else {

                /* If last command is a 'delete' (requiring row shifting) */
                /* Saves the interval of deletion onto redo pile, shifts rows and changes firstEmpty */

                for (i=*firstEmpty-1; i>=ind1; i--) 
                    docHead[i+diff]=docHead[i];

                i=*firstEmpty+diff;
                *firstEmpty=ind1;

                pushStack (ind1, ind2, type, docHead, redoHead, firstEmpty, leftovers, leftoverSize);

                popStack (docHead, undoHead, &count, leftovers, leftoverSize); 

                *firstEmpty=i;
            }

        }  

    }

    return;
}

void actualRedo (char**docHead, int*firstEmpty, row_t**undoHead, row_t**redoHead, int n, row_t**leftovers, int*leftoverSize, int*segCountU, int*segCountR)
{
    /* This function puts into practice the correct number of redos after accounting for undo-redo subtractions */

    int ind2, count;

    /* Executes redo 'n' times, restoring undone commands */

    n=abs(n);

    for (; n>0; n--) {

        if ((*redoHead)->type==COPY) {
 
            ind2 = (*redoHead)->ind2;

            pushStack ((*redoHead)->ind1, ind2, COPY, docHead, undoHead, firstEmpty, leftovers, leftoverSize);

            /* Changes firstEmpty accordingly */

            if (ind2>=*firstEmpty) {
                *firstEmpty=ind2+1; 
            }

        }else
            doDelete(docHead, undoHead, firstEmpty, (*redoHead)->ind1, (*redoHead)->ind2, leftovers, leftoverSize, segCountU, segCountR);
            
        popStack(docHead, redoHead, &count, leftovers, leftoverSize);
        
    }

    return;
}


/* Structure manipulation functions bodies */

void reSize (char***docHead, int*size)
{   
    /* This function expands the size of docHead array in case it isn't sufficient */

    int num;

    /* Creates new docHead array of size equals to previous one + INCREASE amount, and transfers rows between the two */

    if (*size==0)
        num=INCREASE;
    else
        num=(*size)*(*size);

    *docHead=realloc(*docHead, sizeof(char*)*(num));
        
    *size=num;

    return;
}

void pushStack (int ind1, int ind2, _Bool type, char**docHead, row_t**stackHead, int*firstEmpty, row_t**leftovers, int*leftoverSize)  
{   
    /* This function pushes a set interval of document rows onto the desired pile */

    row_t*new;
    int i, j, top, size;
    char**row;

    /* Calls function to make new undo or redo pile element from docHead rows between indexes */
    /* The adds onto redo or undo pile, as dictated by content of 'stackHead' variable */

    if (*leftoverSize>0) {
        (*leftoverSize)--;
        new=*leftovers;
        *leftovers=new->next;
    } else {
        new=malloc(sizeof(row_t));
    }

    if (ind2>=*firstEmpty)
        top=*firstEmpty;
    else
        top=ind2+1;

    if (top-ind1<0)
        size=1;
    else 
        size=top-ind1+1;

    new->row=malloc(sizeof(char*)*(size));
    row=new->row;
    for (i=ind1, j=0; i<top; i++, j++)
        row[j]=docHead[i];
    row[j]=NULL;  

    new->ind1=ind1;
    new->ind2=ind2;
    new->type=type;
    new->next=*stackHead;
    *stackHead=new;

    return;
} 

void popStack (char**docHead, row_t**stackHead, int*count, row_t**leftovers, int*leftoverSize)
{
    /* This function pops an element off of the desired pile and writes its rows onto the document */

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
        docHead[ind1]=row[i];
    }
    newHead=(*stackHead)->next;
    free(row);

    (*leftoverSize)++;
    (*stackHead)->next=*leftovers;
    *leftovers=*stackHead;
    
    *stackHead=newHead;
    *count=i;

    return;
}

void pushSnap (char**docHead, row_t**undoHead, int firstEmpty) 
{
    row_t*new;
    char**rows;
    int i;

    new=malloc(sizeof(row_t));
    new->ind1=-1;
    rows=malloc (sizeof(char*)*(firstEmpty+1));

    for (i=0; i<firstEmpty; i++)
        rows[i]=docHead[i];
    rows[i]=NULL;

    new->row=rows;
    new->next=*undoHead;
    *undoHead=new;

    return;
}

void emptyRedo (row_t**redoHead, row_t**leftovers)
{   
    /* This function empties the entirety of the redo pile (duh) */

    row_t*del, *leftH, *prev;
    char**row;
    int i;

    /* Sequentially deletes redo pile */
    
    if (*redoHead!=NULL) {
        leftH=*leftovers;
        *leftovers=*redoHead;
        *redoHead=NULL;
        for (del=*leftovers, prev=NULL; del!=NULL; prev=del, del=del->next) {

            row=del->row;
            for (i=0; row[i]!=NULL; i++)
                free(row[i]);
            free(row);
        }
        prev->next=leftH;
    }

    return;
}
