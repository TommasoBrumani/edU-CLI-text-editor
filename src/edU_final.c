/* Final Version */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1024+1
#define INCREASE 1000000
#define FSSIZE 5
#define SNAPFREQ 38 
#define ASCIIZERO 48
#define ASCIININE 57
#define ASCIIC 99
#define ASCIIU 117

/* Type for elements of the undo/redo list (each stores a command or a copy of document) */
typedef struct pile_s {
    char**row;
    int ind1;
    int ind2;
    struct pile_s*next;
    struct pile_s*prev;
} pile_t;

/* Command functions */
void doCopy (char***, int*, int*, pile_t**, int*, int, int);
void doDelete (char**, int*, pile_t**, int*, int, int);
void doPrint (char**, int, int, int);
void doUndo (int*, int*, int*, int);
void doRedo (int*, int*, int*, int);

/* Undo and Redo balance functions */
void doBalance (char**, int*, pile_t**, int*, int*);
void actualUndo (char**, int*, pile_t**, int, int*);
void actualRedo (char**, int*, pile_t**, int, int*);

/* Undo and Redo list manipulation functions */
void addPile (int, int, pile_t**);
pile_t*afterSnap (char**, int*, pile_t*, int);
void emptyRedo (pile_t*);

/* Document manipulation functions */
void reSize (char***, int*);


/* MAIN function body */

int main ()
{
    char**docHead=NULL;                 /* Document array (array of strings, every string is a row) */
    pile_t*currCommand=NULL;            /* Pointer to last executed command in undo/redo list */
    int docSize, undoSize, redoSize;    /* Document size, maximum number of executable undos and redos */
    int firstEmpty;                     /* First empty row in document */
    int balance;                        /* Number of undos or redos to be executed after accounting for their subtraction */
    int snapCount;                      /* Number of commands since last "snapshot" (copy of document) */
    int i, j, ind1, ind2;               /* Cicle counters, ind1 and ind2 (or n) */
    char c, command, in [MAX];          /* Character in input, command identifying letter, command string */

    docSize=0;
    undoSize=0;
    redoSize=0;
    firstEmpty=0;
    balance=0;
    snapCount=0;

    currCommand=malloc (sizeof(pile_t));
    currCommand->next=NULL;
    currCommand->prev=NULL;
    currCommand->row=NULL;
    currCommand->ind1=0;
    currCommand->ind2=0;

    do {

        command='0';
        i=0;
        j=0;

        do {

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

            i++;

        } while (c!='\n');

        if (command=='c') {

            doBalance (docHead, &firstEmpty, &currCommand, &balance, &snapCount);
            doCopy (&docHead, &docSize, &firstEmpty, &currCommand, &snapCount, ind1, ind2);
            undoSize++;
            emptyRedo (currCommand);
            redoSize=0;

        } else if (command=='d') {

            doBalance (docHead, &firstEmpty, &currCommand, &balance, &snapCount);
            doDelete (docHead, &firstEmpty, &currCommand, &snapCount, ind1, ind2);
            undoSize++;
            emptyRedo (currCommand);
            redoSize=0;

        } else if (command=='p') {

            doBalance (docHead, &firstEmpty, &currCommand, &balance, &snapCount);
            doPrint (docHead, firstEmpty, ind1, ind2);

        } else if (command=='u') {

            ind2++;
            doUndo (&balance, &undoSize, &redoSize, ind2);

        } else if (command=='r') {

            ind2++;
            doRedo (&balance, &undoSize, &redoSize, ind2);

        }

    } while (command!='q');

    /* printf ("See you, space cowboy.\n"); */

    return 0;
}


/* Command functions bodies */

void doCopy (char***docHead, int*docSize, int*firstEmpty, pile_t**currCommand, int*snapCount, int ind1, int ind2)
{
    char**row, **docH, *tmp;
    char fullStop [FSSIZE];
    int docS, firstE, i;

    i=0;
    docH=*docHead;
    docS=*docSize;

    if (ind2>=*firstEmpty) 
        *firstEmpty=ind2+1; 

    if (*snapCount>=SNAPFREQ) {

        addPile (-(ind1+1), -(ind2+1), currCommand);

        row=malloc (sizeof(char*)*((*firstEmpty)+1));

        for (; i<ind1; i++) {

            row[i]=docH[i];

        }

    } else {

        addPile (ind1+1, ind2+1, currCommand);

        row=malloc (sizeof(char*)*(ind2-ind1+1));

    }

    for (; ind1<=ind2; i++, ind1++) {

        if (docS <= ind1) {
            reSize (docHead, docSize);
            docS=*docSize;
            docH=*docHead;
        }

        tmp=malloc(sizeof(char)*(MAX));
        if (fgets (tmp, MAX, stdin)!=NULL);
        tmp=realloc(tmp, (strlen(tmp)+1));
        

        docH[ind1]=tmp; 
        row[i]=tmp;

    }

    if (fgets (fullStop, FSSIZE, stdin)!=NULL);

    if (*snapCount>=SNAPFREQ) {

        for (firstE=*firstEmpty; i<firstE; i++) 
            row[i]=docH[i];
        row[i]=NULL;

        (*snapCount)=0;

    } else 
        (*snapCount)++;

    row[i]=NULL;

    (*currCommand)->row=row;

    return;
}

void doDelete (char**docHead, int*firstEmpty, pile_t**currCommand, int*snapCount, int ind1, int ind2)
{   
    int i, top, firstE;
    char **row;

    if (ind2>=*firstEmpty) {

        if (ind1<=*firstEmpty) 
            *firstEmpty=ind1;

    } else {

        for (i=ind1, top=ind2+1, firstE=*firstEmpty; top<firstE; i++, top++)
            docHead[i]=docHead[top];

        *firstEmpty=i;
    }

    if (*snapCount>SNAPFREQ) {

        addPile (-(ind1+1), 0, currCommand);

        row=malloc (sizeof(char*)*((*firstEmpty)+2));

        for (i=0; i<ind1; i++) {

            row[i]=docHead[i];

        }

        for (firstE=*firstEmpty; i<firstE; i++) 
            row[i]=docHead[i];

        row[i]=NULL;
        
        (*snapCount)=0;

    } else {

        addPile (ind1+1, ind2+1, currCommand);

        row=NULL;

        (*snapCount)++;

    }

    (*currCommand)->row=row;

    return;
}

void doPrint (char**docHead, int firstEmpty, int ind1, int ind2)
{
    char fullStop [3]= ".\n\0";

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

void doUndo (int*balance, int*undoSize, int*redoSize, int undos)
{
    if(undos<=*undoSize) {

        *balance=*balance+undos;
        *undoSize=*undoSize-undos;
        *redoSize=*redoSize+undos;

    } else {

        *balance=*balance+*undoSize;
        *redoSize=*redoSize+*undoSize;
        *undoSize=0;

    }

    return;
}

void doRedo (int*balance, int*undoSize, int*redoSize, int redos)
{
    if(redos<=*redoSize) {

        *balance=*balance-redos;
        *undoSize=*undoSize+redos;
        *redoSize=*redoSize-redos;

    } else {

        *balance=*balance-*redoSize;
        *undoSize=*undoSize+*redoSize;
        *redoSize=0;

    }

    return;
}


/* Undo and Redo balance */

void doBalance (char**docHead, int*firstEmpty, pile_t**currCommand, int*balance, int*snapCount)
{
    if (*balance>0) {

        actualUndo (docHead, firstEmpty, currCommand, *balance, snapCount);
        *balance=0;

    } else if (*balance<0) {

        actualRedo (docHead, firstEmpty, currCommand, -(*balance), snapCount);
        *balance=0;

    }

    return;
}

void actualUndo (char**docHead, int*firstEmpty, pile_t**currCommand, int undos, int*snapCount)
{
    pile_t*curr;
    char**rows;
    int i, out;

    out=0;

    for (curr=*currCommand; undos>0; undos--, curr=curr->prev);

    for (; curr->ind1>0; curr=curr->prev, undos++);

    if (curr->ind1<0) {

        for (rows=curr->row, i=0; rows[i]!=NULL; i++) 
            docHead[i]=rows[i];

        *firstEmpty=i;

        if (undos>0)
            curr=curr->next;

        undos--;

    } else 
        *firstEmpty=0;

    *snapCount=undos;
    
    curr=afterSnap (docHead, firstEmpty, curr, undos);

    *currCommand=curr;

    return;
}

void actualRedo (char**docHead, int*firstEmpty, pile_t**currCommand, int redos, int*snapCount)
{
    pile_t*curr;
    char**rows;
    int i, out, max;

    out=0;
    max=redos-1;

    for (curr=*currCommand; redos>0; redos--, curr=curr->next);

    for (; curr->ind1>0 && max>0; curr=curr->prev, max--, redos++);

    if (max>0) {

        for (rows=curr->row, i=0; rows[i]!=NULL; i++) 
            docHead[i]=rows[i];

        *firstEmpty=i;
        *snapCount=redos;

        if (redos>0)
            curr=curr->next;

        redos--;

    } else
        *snapCount=*snapCount+redos;

    curr=afterSnap (docHead, firstEmpty, curr, redos);

    *currCommand=curr;

    return;
}


/* Undo and Redo pile manipulation */

void addPile (int ind1, int ind2, pile_t**currCommand)
{
    pile_t*new, *tmp;

    new=malloc (sizeof(pile_t));

    new->ind1=ind1;
    new->ind2=ind2;

    new->prev=*currCommand;
    new->next=NULL;
    (*currCommand)->next=new;
    *currCommand=new;

    return;
}

pile_t*afterSnap (char**docHead, int*firstEmpty, pile_t*curr, int redos)
{   
    char**rows;
    int i, firstE;
    int ind1, ind2;

    for (firstE=*firstEmpty; redos>=0; redos--) {

        rows=curr->row;
        ind1=(curr->ind1)-1;
        ind2=(curr->ind2)-1;

        if (rows!=NULL) {

            for (i=0; ind1<=ind2; i++, ind1++)
                docHead[ind1]=rows[i];
            
            if (ind2>=firstE) 
                firstE=ind2+1; 

        } else {

            if (ind2>=firstE) {

                if (ind1<=firstE) 
                    firstE=ind1;

            } else {

                for (i=ind2+1; i<firstE; ind1++, i++)
                    docHead[ind1]=docHead[i];

                firstE=ind1;

            }

        }

        if (redos>0)
            curr=curr->next;

    }

    *firstEmpty=firstE;

    return curr;
}

void emptyRedo (pile_t*curr)
{   
    pile_t*start, *next;
    char**rows;
    int ind1, ind2, i;

    start=curr;

    for (curr=curr->next; curr!=NULL; curr=next) {

        next=curr->next;
        rows=curr->row;

        if (rows!=NULL) {

            if (curr->ind1>0) {

                ind1=(curr->ind1)-1;
                ind2=(curr->ind2)-1;
                i=0;

            } else {

                ind1=-((curr->ind1)-1);
                if (curr->ind2!=0)
                    ind2=-((curr->ind2)-1);
                else
                    ind2=0;
                i=ind1;
            
            }

            if (ind2!=0) {

                for (; ind1<=ind2; ind1++, i++)
                    free (rows[i]);
            
            }
            
            free(rows);

        }

        free(curr);

    }

    start->next=NULL;

    return;
}


/* Document manipulation */

void reSize (char***docHead, int*size)
{   
    /* This function expands the size of docHead array in case it isn't sufficient */

    int num;

    if (*size==0)
        num=INCREASE;
    else
        num=(*size)*(*size);

    *docHead=realloc(*docHead, sizeof(char*)*(num));
        
    *size=num;

    return;
}

