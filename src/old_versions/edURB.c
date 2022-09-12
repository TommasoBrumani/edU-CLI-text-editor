#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#define MAX 1024+1
#define FSSIZE 5
#define BLACK 1
#define RED 0

/* Type for document (Red and Black tree) nodes */
typedef struct node_s {
    char*row;
    int ind;
    _Bool color;
    struct node_s*parent;
    struct node_s*right;
    struct node_s*left;
} node_t;

/* Type for undo and redo pile nodes */
typedef struct pile_s {
    char**rows;
    int ind1;
    int ind2;
    struct pile_s*next;
} pile_t;


/* Input functions */
void menu ();
char* input ();
void readIndex (char[], int*, int*);
int readNumber (char[]);

/* Command execution functions */
void eduC (node_t**, pile_t**, pile_t**, char*, node_t*);
void eduD (node_t**, pile_t**, pile_t**, char*, node_t*);
void eduP (node_t**, char*);
void eduU (node_t**, pile_t**, pile_t**, char*);
void eduR (node_t**, pile_t**, pile_t**, char*);

/* Binary Search Tree operations functions */
node_t* search (node_t*, int);
node_t* min (node_t*);
node_t* max (node_t*);
node_t* next (node_t*);
void insert (node_t**, node_t*);


/* Red and Black tree manipulation functions */
void leftRotate (node_t**, node_t*);
void rightRotate (node_t**, node_t*);
void repairRBinsert (node_t**, node_t*);
void repairRBdelete (node_t**, node_t*);


/* Input functions bodies */

int main (int argc, char*argv[])
{   
    char*in;
    int i;
    node_t*docHead=NULL;
    node_t*leaf=NULL;
    pile_t*undoHead=NULL;
    pile_t*redoHead=NULL;

    leaf=malloc (sizeof(node_t));
    leaf->ind=0;
    leaf->parent=NULL;
    leaf->left=NULL;
    leaf->right=NULL;
    leaf->color = BLACK;

    in=input ();
    
    for (i=0; *(in+i)!='\0' && in[i]!='q';) {
        if (in[i]=='c'){
            eduC(&docHead, &undoHead, &redoHead, in, leaf);
            in=input ();
            i=0;
        }else if (in[i]=='d'){
            eduD(&docHead, &undoHead, &redoHead, in, leaf);
            in=input ();
            i=0;
        }else if (in[i]=='p'){
            eduP(&docHead, in);
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


/* Command execution functions bodies */

void eduC (node_t **docHead, pile_t**undoHead, pile_t**redoHead, char*in, node_t*leaf) 
{
    int ind1, ind2;
    pile_t*new;
    node_t*start;

    readIndex(in, &ind1, &ind2);
    free(in);

    /* actual undo */

    overWrite ();

    /* empty redo */

    return;
}

void eduD (node_t**, pile_t**, pile_t**, char*, node_t*)
{
    return;
}

void eduP (node_t**, char*)
{
    return;
}

void eduU (node_t**, pile_t**, pile_t**, char*)
{
    return;
}

void eduR (node_t**, pile_t**, pile_t**, char*)
{
    return;
}


/* Binary Search Tree operations functions */

node_t* search (node_t*curr, int key)
{
    if (curr->ind==0 || curr->ind==key) 
        return curr;

    if (curr->ind<key)
        return search(curr->right, key);

    return search (curr->left, key);    
}

node_t* min (node_t*head)
{   
    node_t* curr;

    curr=head;
    while (curr->left->ind!=0)
        curr=curr->left;
    
    return curr;
}

node_t* max (node_t*head)
{
    node_t*curr;

    curr=head;
    while (curr->right->ind!=0)
        curr=curr->right;
    
    return curr;
}

node_t* next (node_t*x)
{
    node_t*y;

    if (x->right->ind!=0)
        return min(x->right);

    y=x->parent;
    
    while (y->ind!=0 && y->right==x) {
        x=y;
        y=y->parent;
    }

    return y;
}

void insert (node_t**head, node_t*x)
{
    node_t *curr, *pre;

    pre=NULL;
    curr=*head;

    while (curr->ind!=0) {

        pre=curr;

        if (x->ind<curr->ind)
            curr=curr->left;
        else
            curr=curr->right;

    }

    x->parent=pre;

    if (pre->ind!=0)
        *head=x;
    else if (x->ind<pre->ind)
        pre->left=x;
    else
        pre->right=x;

    return;
}

node_t* delete (node_t**head, node_t*x)
{
    node_t*del;
    node_t*under;

    if (x->left->ind==0 || x->right->ind==0) 
        del=x;
    else
        del=next(x);
    
    if (del->left->ind==0)
        under=del->left;
    else
        under=del->right;

    if (under->ind!=0)
        under->parent=del->parent;
    
    if (del->parent->ind==0)
        *head=under;
    else if (del==del->parent->left)
        del->parent->left=under;
    else
        del->parent->right=under;

    if (del!=x)
        x->ind=del->ind;

    free(del);

    return del;
}


/* Red and Black tree manipulation functions bodies */

void leftRotate (node_t**head, node_t*x)
{   
    node_t*y;

    y=x->right;
    x->right=y->left;

    if (y->left->ind!=0)
        y->left->parent=x;

    y->parent=x->parent;

    if (x->parent->ind==0)
        *head=y;
    else if (x==x->parent->left)
        x->parent->left=y;
    else 
        x->parent->right=y;
    
    y->left=x;
    x->parent=y;

    return;
}

void rightRotate (node_t**head, node_t*x)
{
    node_t*y;

    y=x->left;
    x->left=y->right;

    if (y->right->ind!=0)
        y->right->parent=x;

    y->parent=x->parent;

    if (x->parent->ind==0)
        *head=y;
    else if (x==x->parent->right)
        x->parent->right=y;
    else 
        x->parent->left=y;
    
    y->right=x;
    x->parent=y;

    return;
}

void repairRBinsert (node_t**head, node_t*z)
{   
    node_t *x, *y;

    while (z->parent->color==RED) {

        x=z->parent;

        if (x==x->parent->left) {

            y=x->parent->right;

            if (y->color==RED) {

                x->color=BLACK;
                y->color=BLACK;
                x->parent->color=RED;
                z=x->parent;

            } else {
                
                if (z=x->right) {
                    z=x;
                    leftRotate (head, z);
                    x=z->parent;
                }

                x->color=BLACK;
                x->parent->color=RED;
                rightRotate (head, x->parent);

            }
        } else {

            y=x->parent->left;

            if (y->color==RED) {

                x->color=BLACK;
                y->color=BLACK;
                x->parent->color=RED;
                z=x->parent;

            } else {

                if (z=x->left) {
                    z=x;
                    rightRotate (head, z);
                    x=z->parent;
                }

                x->color=BLACK;
                x->parent->color=RED;
                leftRotate (head, x->parent);
                
            }

        }

    }
    (*head)->color=BLACK;

    return;
}

void repairRBdelete (node_t**head, node_t*x)
{   
    node_t*w;

    if (x->color==RED)  

        x->color=BLACK;

    else {

        if (x==x->parent->left) {

            w=x->parent->right;

            if (w->color==RED) {
                w->color=BLACK;
                w->parent->color=RED;
                leftRotate(head, w->parent);
                w=x->parent->right;
            }

            if (w->left->color==BLACK && w->right->color==BLACK) {

                w->color=RED;
                repairRBdelete(head, x->parent);


            } else {

                if (w->right->color==BLACK) {

                    w->color=RED;
                    w->left->color=BLACK;
                    rightRotate(head, w);
                    w=x->parent->right;

                }

                w->color=w->parent->color;
                w->right=BLACK;
                leftRotate(head, w->parent);

            }

        } else {

            w=x->parent->left;

            if (w->color==RED) {
                w->color=BLACK;
                w->parent->color=RED;
                rightRotate(head, w->parent);
                w=x->parent->left;
            }

            if (w->right->color==BLACK && w->left->color==BLACK) {

                w->color=RED;
                repairRBdelete(head, x->parent);


            } else {

                if (w->left->color==BLACK) {

                    w->color=RED;
                    w->right->color=BLACK;
                    leftRotate(head, w);
                    w=x->parent->left;

                }

                w->color=w->parent->color;
                w->left=BLACK;
                rightRotate(head, w->parent);

            }

        }

    }

    return;
}
