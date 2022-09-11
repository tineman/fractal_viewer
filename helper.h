#ifndef _HELPER
#define _HELPER

//Structs and constants used everywhere
//Also includes the functions used for setting up SDL and calculating the colour of each point

#define WIDTH 480
#define HEIGHT 480

#define ITERATIONS 30

#include <math.h>

//SDL components
typedef struct Backend
{
    SDL_Window* p_window;
    SDL_Renderer* p_renderer;
} Backend;

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



/*
    INITIALISES p_window and p_renderer and sets them up

    \param p_window The pointer to where the window will be stored
    \param p_renderer The pointer to where the renderer will be stored
*/
Backend init_backend();



/*
    FREES p_window and p_renderer

    \param p_window The pointer to where the window will be stored
    \param p_renderer The pointer to where the renderer will be stored
*/
void del_backend(SDL_Window* p_window, SDL_Renderer* p_renderer);



/*
    RETURNS the number of iterations it takes for query to escape. Return 0 if query does not escape (arbitrary decision to make colouring easier)

    \param query - the coordinate in question
*/
int escape(Coord query);



/*
    RENDERS the mandelbrot set
    Warning: max.real:max.imag :: WIDTH:HEIGHT, otherwise the fractal will be stretched/compressed

    \param p_renderer The renderer on which the function renders
    \param max The largest coordinate on the screen
    \param mid The coordinate at the centre of the screen
*/
void render(SDL_Renderer* p_renderer, Coord max, Coord mid);



/*
    CHANGES array colour such that the subarray bounded by Pixel top_left, bottom_right countains the colours to be drawn to the screen

    Note - this function renders by calling escape on progressvely smaller rectangles. This is based on the fact that if the perimeter of an enclosed area
    on the mandelbrot set is coloured in one colour, everything in that area can be coloured in that colour. (Mariani-Silver algorithm)

    \param color The array assigning a colour to the corresponding pixel on the screen
    \param top_left The top left pixel bounding the area to be subrendered (i.e. the pixel with the smallest x, y fields)
    \param bottom_right The bottom right pixel bounding the area to be subrendered (i.e. the pixel with the largest x, y fields)
    \param line A point defining the line which must be evaluated. The point is one unit away from either top_left or bottom_right
    \param scale A coord representing the number of cartesian points a pixel represents
    \param eval_line A boolean. True if the new line hsa not been calculated. False if it has
*/
void subrender(Uint8 color[HEIGHT][WIDTH], Pixel top_left, Pixel bottom_right, Pixel line, Coord scale, int eval_line, Coord max, Coord mid);



/*
    PANS the current camera when the left mouse button is pressed. The function
    pans the current X and Y coordinates of the screen at a one to one ratio to 
    the distance the mouse moves. The function exits when the mouse button is
    released.

    \param init The mouse's pixel coordinates at the time the mouse is pressed
    \param max The magnitude of the area being rendered
    \param p_mid The pointer to the current midpoint
*/
void pan(SDL_Renderer* p_renderer, Pixel init, Coord max, Coord* p_mid);



#endif // #ifndef _HELPER