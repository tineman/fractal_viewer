#ifndef _HELPER
#define _HELPER

//Structs and constants used everywhere

#define WIDTH 480
#define HEIGHT 480

//Framerate is the number of frames per second. Recommended to be at least 60 to prevent
//frames from "stuttering"
#define FRAMERATE 90
/*
    The number of colours that show up on screen. The recommended is 5, 6, or 7.
    In general, the more you want to zoom in, the higher it should be.
    1 <= Palette_depth <= 7
*/
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

//Index starts at one
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