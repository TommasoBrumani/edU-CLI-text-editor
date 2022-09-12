/* This version uses lists */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#define MAX 1024+1
#define FSSIZE 5
#define COPY 1
#define DELETE 0

typedef struct node_s {
    char*row;
    struct node_s*next;
} node_t;

typedef struct pile_s {
    char**rows;
    int ind1;
    int ind2;
    struct pile_s*next;
} pile_t;

char* input ();
void readIndex (char[], int*, int*);
int readNumber (char[]);

void eduC (node_t**, pile_t**, pile_t**, char*);
void eduD (node_t**, pile_t**, pile_t**, char*);
void eduP (node_t*, char*);
void eduU (node_t**, pile_t**, pile_t**, char*);
void eduR (node_t**, pile_t**, pile_t**, char*);

void empty (pile_t**);


int main (int argc, char*argv[])
{   
    char*in;
    int i;
    node_t*docHead=NULL;
    pile_t*undoHead=NULL;
    pile_t*redoHead=NULL;

    in=input ();
   
    for (i=0; *(in+i)!='\0' && in[i]!='q';) {
        if (in[i]=='c'){
            eduC(&docHead, &undoHead, &redoHead, in);
            in=input ();
            i=0;
        }else if (in[i]=='d'){
            eduD(&docHead, &undoHead, &redoHead, in);
            in=input ();
            i=0;
        }else if (in[i]=='p'){
            eduP(docHead, in);
            in=input ();
            i=0;
        }else if (in[i]=='u'){
            eduU(&docHead, &undoHead, &redoHead, in);
            in=input ();
            i=0;
        }else if (in[i]=='r'){
            eduR(&docHead, &undoHead, &redoHead, in);
            in=input ();
            i=0;
        }else
            i++;

    }

    /*printf ("See you, Space Cowboy.\n");*/
    
    return 0;
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

    in=malloc(sizeof(char)*(size));
    in=strcpy(in, tmp);

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

void eduC (node_t**docHead, pile_t**undoHead, pile_t**redoHead, char*in) 
{
    int ind1, ind2, i;
    _Bool check;
    char**rows;
    char fullStop [FSSIZE];
    pile_t*new;
    node_t*curr, *prev;
    
    readIndex(in, &ind1, &ind2);
    free(in);
    
    new=malloc(sizeof(pile_t));
    rows=malloc(sizeof(char*)*(ind2-ind1+2));
    new->ind1=ind1;
    new->ind2=ind2;
    check=0;

    for (i=0, curr=*docHead; i<ind1 && curr!=NULL; i++, prev=curr, curr=curr->next);

    for (i=0; ind1<=ind2 && curr!=NULL; ind1++, i++, prev=curr, curr=curr->next) {
        rows[i]=curr->row;
        curr->row=input();
    }

    if (*docHead==NULL) {
        curr=malloc(sizeof(node_t));
        curr->row=input();
        *docHead=curr;
        check=1;
        ind1++;
        prev=curr;
    } else if (ind1<=ind2)
        check=1;

    for (rows[i]=NULL; ind1<=ind2; ind1++, prev=curr) {
        curr=malloc(sizeof(node_t));
        curr->row=input();
        prev->next=curr;
    }

    if (check!=0)
        prev->next=NULL;

    new->rows=rows;
    new->next=*undoHead;
    *undoHead=new;

    fgets (fullStop, FSSIZE, stdin);

    empty(redoHead);

    return;
}

void eduD (node_t**docHead, pile_t**undoHead, pile_t**redoHead, char*in) 
{
    return;
}

void eduP (node_t*docHead, char*in) 
{
    int ind1, ind2, i;
    char fullStop [3]= ".\n\0";
    node_t*curr;

    readIndex(in, &ind1, &ind2);
    free(in);

    for (i=0, curr=docHead; i<ind1; i++, curr=curr->next)
        ;

    for (; ind1<0; ind1++) {
        fputs(fullStop, stdout);
    }

    for (; ind1<=ind2 && curr!=NULL; ind1++, curr=curr->next) {
        fputs(curr->row, stdout);
    }

    for (; ind1<=ind2; ind1++) {
        fputs(fullStop, stdout);
    }

    return;
}

void eduU (node_t**docHead, pile_t**undoHead, pile_t**redoHead, char*in)
{
    return;
}

void eduR (node_t**docHead, pile_t**undoHead, pile_t**redoHead, char*in)
{
    return;
}

void empty (pile_t**redoHead)
{
    pile_t*del, *reH;
    char**rows;
    int i;

    reH=*redoHead;
    while (reH!=NULL) {
        del=reH;
        rows=del->rows;
        for (i=0; rows[i]!=NULL; i++)
            free(rows[i]);
        free(rows);
        reH=(reH)->next;
        free(del);
    }
    *redoHead=reH;

    return;
}
