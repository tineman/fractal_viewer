#include "helper.h"

Panel_Node* newPanel(Coord max, Coord mid, int duration)
{
    Panel_Node* root = (Panel_Node*) malloc(sizeof(Panel_Node));
    root->max = max;
    root->mid = mid;
    root->duration = duration;
    root->next = NULL;
    return root;
}

Panel_Node* addPanel(Panel_Node* root, Panel_Node* new, int index)
{
    if(root == NULL) return new;

    if(index == 1) //head
    {
        new->next = root;
        return new;
    }

    Panel_Node* old_root = root;

    while(index != 2)
    {
        index--;
        root = root->next;
    }

    new->next = root->next;
    root->next = new;
    return old_root;

}

//RETURN the root of a linked list with a panelnode deleted at index index
Panel_Node* deletePanel(Panel_Node* root, int index)
{
    //free
    if(root == NULL) return NULL;

    Panel_Node* new_panel;

    if(index == 1) //head
    {
        new_panel = root->next;
        free(root);
        return new_panel;
    }

    Panel_Node* old_root = root;

    while(index != 2)
    {
        index--;
        root = root->next;
    }

    new_panel = root->next->next;
    free(root->next);
    root->next = new_panel;
    return old_root;
}

//PRINT the information in the linked list pointed to by root
void printInfo(Panel_Node* root)
{
    int index = 1;

    if(root == NULL) printf("There are no panels yet.\n");

    while(root != NULL)
    {
        printf("PANEL %d\n", index); 
        printf("Midpoint: (%Lf, %Lf)\n", root->mid.real, root->mid.imag);
        printf("Maxpoint: (%Lf, %Lf)\n", root->max.real, root->max.imag);
        printf("Duration: %d seconds\n\n", root->duration);
        index++;
        root = root->next;
    }
}
/* Testing
int main()
{

    Panel_Node* root = NULL;
    printInfo(root);

    Coord max = {.real = 0, .imag = 0};    
    Coord mid = {.real = 0, .imag = 0};    
    root = addPanel(root, newPanel(max, mid, 10), 1);
    printInfo(root);

    max.real = 1;
    max.imag = 1;
    mid.real = 1;
    mid.imag = 1;
    root = addPanel(root, newPanel(max, mid, 10), 2);
    printInfo(root);

    max.real = 2;
    max.imag = max.real;
    mid.real = max.imag;
    mid.imag = mid.real;
    root = addPanel(root, newPanel(max, mid, 10), 3);

    max.real = 3;
    max.imag = max.real;
    mid.real = max.imag;
    mid.imag = mid.real;
    root = addPanel(root, newPanel(max, mid, 10), 4);

    max.real = 4;
    max.imag = max.real;
    mid.real = max.imag;
    mid.imag = mid.real;
    root = addPanel(root, newPanel(max, mid, 10), 5);

    max.real = 5;
    max.imag = max.real;
    mid.real = max.imag;
    mid.imag = mid.real;
    root = addPanel(root, newPanel(max, mid, 10), 6);

    printf("\n\n\n Test 1\n");
    printInfo(root);

    //0 1 2 3 4 5

    max.real = 10;
    max.imag = max.real;
    mid.real = max.imag;
    mid.imag = mid.real;
    root = addPanel(root, newPanel(max, mid, 10), 1);
    printf("\n\n\n Test 2\n");
    printInfo(root);

    //10 0 1 2 3 4 5

    max.real = 11;
    max.imag = max.real;
    mid.real = max.imag;
    mid.imag = mid.real;
    root = addPanel(root, newPanel(max, mid, 10), 2);
    printf("\n\n\n Test 3\n");
    printInfo(root);

    //10 11 0 1 2 3 4 5

    max.real = 12;
    max.imag = max.real;
    mid.real = max.imag;
    mid.imag = mid.real;
    root = addPanel(root, newPanel(max, mid, 10), 8);
    printf("\n\n\n Test 4\n");
    printInfo(root);

    //10 11 0 1 2 3 4 12 5

    root = deletePanel(root, 1);
    printf("\n\n\n Test 5\n");
    printInfo(root);

    //11 0 1 2 3 4 12 5

    root = deletePanel(root, 4);
    printf("\n\n\n Test 6\n");
    printInfo(root);

    //11 0 1 3 4 12 5

    root = deletePanel(root, 7);
    printf("\n\n\n Test 7\n");
    printInfo(root);

    //11 0 1 3 4 12

    return 0;
    
}*/