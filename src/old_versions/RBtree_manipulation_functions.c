#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#define BLACK 1
#define RED 0

typedef struct node_s {
    char*row;
    int ind;
    _Bool color;
    struct node_s*parent;
    struct node_s*right;
    struct node_s*left;
} node_t;

node_t* search (node_t*, int);
node_t* min (node_t*);
node_t* max (node_t*);
node_t* next (node_t*);
void insert (node_t**, node_t*);
void delete (node_t**, node_t*)

void leftRotate (node_t**, node_t*);
void rightRotate (node_t**, node_t*);
void repairRBinsert (node_t**, node_t*);
void repairRBdelete (node_t**, node_t*);


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

void delete (node_t**head, node_t*x)
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

    return;
}

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
