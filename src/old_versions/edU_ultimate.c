/* This version uses the super cool structure */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#define MAX 1024+1
#define INCREASE 1000000
#define FSSIZE 5
#define COPY 1
#define DELETE 0
#define ASCIIZERO 48
#define ASCIININE 57

typedef struct row_s {
    char*row;
    _Bool type;
    struct row_s*next;
    struct row_s*prev;
} row_t;

typedef struct pile_s {
    int ind1;
    int ind2;
    _Bool type;
    struct pile_s*next;
} pile_t;

/* Command execution functions */
void doCopy (row_t***, int*, int*, int*, int, int, pile_t**);
void doDelete (row_t**, int*, int, int, int, pile_t**);
void doPrint (row_t**, int, int, int);
void doUndo (int*, int*, int*, int);
void doRedo (int*, int*, int*, int);

/* Undo/Redo balance functions */
void doBalance (row_t**, int*, pile_t**, pile_t**, int);
void actualUndo (row_t**, int*, pile_t**, pile_t**, int);
void actualRedo (row_t**, int*, pile_t**, pile_t**, int);


/* Structure manipulation functions */
void reSize (row_t***, int*);
void addRow (row_t**, int, int);
void addStack (pile_t**, int, int);
void emptyRedo (pile_t**);
char* input ();


/* MAIN */

int main ()
{   
    int i, j;
    int ind1, ind2;
    int firstEmpty, firstUnused, docSize;
    int undoSize, redoSize, n;
    char in[MAX], command;
    row_t**docHead=NULL;
    pile_t*undoHead=NULL;
    pile_t*redoHead=NULL;

    firstEmpty=0;
    firstUnused=0;
    docSize=0;
    undoSize=0;
    redoSize=0;
    n=0;

    fgets (in, MAX, stdin); 
    
    for (i=0, j=0; in[i]!='\n';) {

        if ((in[i]>=ASCIIZERO && in[i]<=ASCIININE) || in[i]=='q') {

            i++;
        
        } else if (in[i]==',') {

            in[i]='\0';
            ind1=atoi(in);
            ind1--;
            i++;
            j=i;
            
        } else {

            command=in[i];
            in[i]='\0';
            ind2=atoi(in+j);

            if (command=='c'){

                ind2--;
                doBalance(docHead, &firstEmpty, &undoHead, &redoHead, n);
                n=0;
                emptyRedo(&redoHead);
                redoSize=0;
                doCopy(&docHead, &docSize, &firstEmpty, &firstUnused, ind1, ind2, &undoHead);
                undoSize++;

            } else if (command=='d'){

                ind2--;
                doBalance(docHead, &firstEmpty, &undoHead, &redoHead, n);
                n=0;
                emptyRedo(&redoHead);
                redoSize=0;
                doDelete(docHead, &firstEmpty, firstUnused, ind1, ind2, &undoHead);
                undoSize++;
    
            } else if (command=='p'){

                ind2--;
                doBalance(docHead, &firstEmpty, &undoHead, &redoHead, n);
                n=0;
                doPrint(docHead, firstEmpty, ind1, ind2);

            } else if (command=='u'){

                doUndo(&n, &undoSize, &redoSize, ind2);
               
            } else if (command=='r'){

                doRedo(&n, &undoSize, &redoSize, ind2);
               
            }

            fgets (in, MAX, stdin);
            i=0;
            j=0;

        }   
    }
    
    return 0;
}


/* Command execution functions bodies */

void doCopy (row_t***docHead, int*docSize, int*firstEmpty, int*firstUnused, int ind1, int ind2, pile_t**undoHead) 
{
    int i, docS;
    char fullStop [FSSIZE];
    row_t** docH;

    for (i=ind1, docS=*docSize, docH=*docHead; i<=ind2; i++) {

        if (docS<=i) {
            reSize (docHead, docSize);
            docH=*docHead;
            docS=*docSize;
        }
        addRow (docH, *firstUnused, i);
        (docH[i])->row=input();

    }

    fgets (fullStop, FSSIZE, stdin);

    if (ind2>=*firstEmpty) 
        *firstEmpty=ind2+1; 

    if (ind2>=*firstUnused)
        *firstUnused=ind2+1;

    addStack (undoHead, ind1, ind2);
    (*undoHead)->type=COPY;

    return;
}

void doDelete (row_t**docHead, int*firstEmpty, int firstUnused, int ind1, int ind2,  pile_t**undoHead) 
{
    int i, top, firstE;
    
    
    firstE=*firstEmpty;

    if (ind2>=firstE) {

        if (ind1<firstE) {

            for (i=ind1; i<=ind2; i++) {

                addRow (docHead, firstUnused, i);
                (docHead[i])->row=NULL;

            }

            *firstEmpty=ind1;
            addStack (undoHead, ind1, firstE-1);

        } else {

            addStack (undoHead, -1, -1);

        }

    } else {

        for (i=ind1, top=ind2+1; top<firstE; i++, top++) {

            addRow (docHead, firstUnused, i);
            docHead[i]->row=docHead[top]->row;

        }

        *firstEmpty=i;
        addStack (undoHead, ind1, ind2);

    }

    (*undoHead)->type=DELETE;

    return;
}

void doPrint (row_t**docHead, int firstEmpty, int ind1, int ind2)
{   
    char fullStop [3]= ".\n\0";


    for (; ind1<0; ind1++) {
        fputs (fullStop, stdout);
    }
    for (; ind1<=ind2 && ind1<firstEmpty; ind1++) {
        fputs (docHead[ind1]->row, stdout);
    }
    for (; ind1<=ind2; ind1++) {
        fputs (fullStop, stdout);
    }

    return;
}

void doUndo (int*n, int*undoSize, int*redoSize, int undos) 
{
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


/* Undo/Redo balance functions */

void doBalance (row_t**docHead, int*firstEmpty, pile_t**undoHead, pile_t**redoHead, int n)
{
    if (n>0) 
        actualUndo (docHead, firstEmpty, undoHead, redoHead, n);
    else if (n<0) 
        actualRedo (docHead, firstEmpty, undoHead, redoHead, n);

    return;
}

void actualUndo (row_t**docHead, int*firstEmpty, pile_t**undoHead, pile_t**redoHead, int n) 
{   

    pile_t *tmp;
    int ind1, ind2, firstE;
    int diff;

    for (; n>0; n--) {

        tmp=*undoHead;
        ind1=tmp->ind1;
        ind2=tmp->ind2;
        firstE=*firstEmpty;

        if (tmp->type==COPY) {

            for (; ind1<=ind2 && docHead[ind1]->prev!=NULL && docHead[ind1]->prev->row!=NULL; ind1++) {

                docHead[ind1]->type=COPY;
                docHead[ind1]=docHead[ind1]->prev;

            }

            if (ind1<=ind2)
                *firstEmpty=ind1;

        } else if (ind1!=-1) {

            if (firstE<=ind1) {
                
                for (; ind1<=ind2 && docHead[ind1]->prev!=NULL; ind1++) {

                    docHead[ind1]->type=DELETE;
                    docHead[ind1]=docHead[ind1]->prev;

                }

                *firstEmpty=ind2+1;
                
            } else {

                diff=ind2-ind1+1;

                for (; ind1<firstE; ind1++) {

                    docHead[ind1]->type=DELETE;
                    docHead[ind1]=docHead[ind1]->prev;

                }

                *firstEmpty=firstE+diff;

            }

        }

        *undoHead=tmp->next;
        tmp->next=*redoHead;
        *redoHead=tmp;

    }

    return;
}

void actualRedo (row_t**docHead, int*firstEmpty, pile_t**undoHead, pile_t**redoHead, int n) 
{
    pile_t *tmp;
    int ind1, ind2, firstE;
    int diff;

    n=abs(n);

    for (; n>0; n--) {

        tmp=*redoHead;
        ind1=tmp->ind1;
        ind2=tmp->ind2;
        firstE=*firstEmpty;

        if (tmp->type==COPY) { 
            
            for (; ind1<=ind2 && ind1<firstE; ind1++) 
                docHead[ind1]=docHead[ind1]->next;

            if (firstE<ind2)
                *firstEmpty=ind2+1;


        } else if (ind1!=-1) {

            if (ind2+1<=firstE) {

                for (; ind1<=ind2 && ind1<firstE; ind1++) 
                    docHead[ind1]=docHead[ind1]->next;
                
                *firstEmpty=ind1;

            } else {  

                diff=ind2-ind1+1;

                for (; ind1<firstE; ind1++) {

                    docHead[ind1]=docHead[ind1]->next;

                }   

                *firstEmpty=firstE-diff;

            }

        }

        *redoHead=tmp->next;
        tmp->next=*undoHead;
        *undoHead=tmp;

    }

    return;
}



/* Structure manipulation functions bodies */

void reSize (row_t***docHead, int*docSize) 
{   
    int num;

    if (*docSize==0)
        num=INCREASE;
    else
        num=(*docSize)*(*docSize);

    *docHead=realloc (*docHead, sizeof(row_t**)*(num));

    *docSize=num;

    return;
}

void addRow (row_t**docHead, int firstUnused, int i)
{   
    row_t*new;

    if (i>=firstUnused) {

        docHead[i]=malloc (sizeof(row_t));
        (docHead[i])->next=NULL;
        (docHead[i])->prev=NULL;

    } else {

        if ((docHead[i])->next!=NULL) {

            if ((docHead[i])->next->type==COPY)
                free(docHead[i]->next->row);

            docHead[i]=(docHead[i])->next;

        } else {

            new=malloc (sizeof(row_t));
            new->next=NULL;
            new->prev=docHead[i];
            (docHead[i])->next=new;
            docHead[i]=new;

        }

    } 

    return;
}

void addStack (pile_t**stackHead, int ind1, int ind2)
{   
    pile_t*new;

    new=malloc (sizeof(pile_t));
    new->ind1=ind1;
    new->ind2=ind2;
    new->next=*stackHead;
    *stackHead=new;

    return;
}

void emptyRedo (pile_t**redoHead)
{
    pile_t *del, *curr;

    curr=*redoHead;
    
    while (curr!=NULL) {

        del=curr;
        curr=curr->next;
        free(del);

    }

    *redoHead=NULL;

    return;
}

char* input () 
{
    char tmp [MAX];
    char*in;
    size_t len;
    int size, i;

    fgets (tmp, MAX, stdin);

    len=strlen(tmp);
    size=len+1;

    in=malloc(sizeof(char)*(size));

    in=strcpy(in, tmp);

    return in;
}
