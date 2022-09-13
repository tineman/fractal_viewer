#ifndef _HELPER
#define _HELPER

//Structs and constants used everywhere
//Also includes the functions used for setting up SDL and calculating the colour of each point

#define WIDTH 480
#define HEIGHT 480

#define FRAMERATE 12
#define PALETTE_DEPTH 7

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

//Struct for pixel information
typedef struct Pixel
{
    int x;
    int y;
} Pixel;

//Struct for cartesian coordinates
typedef struct Coord
{
    long double real;
    long double imag;
} Coord;

// ------ Linked List -------- //

typedef struct panel_ll
{
    Coord max;
    Coord mid;
    int duration;
    struct panel_ll* next;
} Panel_Node;

//RETURN a new panel with a NULL next pointer
Panel_Node* newPanel(Coord max, Coord mid, int duration);

//RETURN the root of a linked list with a panelnode added at index. index Assumes index is valid.
Panel_Node* addPanel(Panel_Node* root, Panel_Node* new, int index);

//RETURN the root of a linked list with a panelnode deleted at index index. Assumes index is valid.
Panel_Node* deletePanel(Panel_Node* root, int index);

//PRINT the information in the linked list pointed to by root
void printInfo(Panel_Node* root);

#endif // #ifndef _HELPER