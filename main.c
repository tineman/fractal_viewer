/*

gcc -Wall -Wextra -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 main.c

App that allows the user to zoom in and pan around the mandelbrot set fractal.

    -

TODO

    - Port to C and SDL
    - Make panning and zooming mouse operations
    - Optimize panning
    - Test the detail given by larger numbers of iterations (what if you use doubles? is there a way to get arbitray precision?)
    - Test with small numbers, see if you can increase amt zoomable (what is the max zoomability given current precision?)
    - Optimize (diff. plotting algoriths?) [definitly]
    - Test for bugs
    - Colouring
    [MVP]
    (gif generator? the user moves to points and they get a gif?) (yes, put on youtube github and website if you cant put it on a website)

    - Read more about associated Julia sets?

*/

#include <stdio.h>
#include <math.h> //needed for the pow operator because apparently C doens't have exponents!??!?!?!!?
#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 480

#define ITERATIONS 30

typedef struct Backend
{
    SDL_Window* p_window;
    SDL_Renderer* p_renderer;
} Backend;

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
    MODIFIES float at pointer truple such that ... using a brute force algorithm

    \param a The real value of z
    \param b The imaginary value of z
    \param triple is the pointer to the first element of the array of RGB values
*/
void escape(float a, float b, Uint8* triple);

/*
    RENDERS the mandelbrot set such that

    \param p_renderer The renderer on which the function renders
    \param max_x The largest cartesian x-value on the screen
    \param max_y The largest cartesian y-value on the screen
    \param mid_x The cartesian x-value at the center of the screen
    \param mid_y The cartesian y-value at the center of the screen
*/
void render(SDL_Renderer* p_renderer, float max_x, float max_y, float mid_x, float mid_y);

Backend init_backend()
{
    SDL_Init(SDL_INIT_VIDEO);

    Backend backend;

    backend.p_window = SDL_CreateWindow("Fractal Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    backend.p_renderer = SDL_CreateRenderer(backend.p_window, -1, SDL_RENDERER_ACCELERATED);

    return backend;
}

void del_backend(SDL_Window* p_window, SDL_Renderer* p_renderer)
{
    SDL_RenderClear(p_renderer);
    SDL_DestroyWindow(p_window);
    SDL_Quit();
}

void escape(float a, float b, Uint8* triple)
{
    float ainit = a;
    float binit = b;

    for(float i = 0; i < ITERATIONS; i++) //perhaps change iterations dynamically? //note - if i is int, (i/ITERATIONS) returns an integer, 0
    {
        if(a > 2 || b > 2)
        {
            //note - with the current colouring scheme, only the first element of the array is changed, the other values are initialised to 0
            *(triple) = (Uint8) ((255 * (i/ITERATIONS))); //perhaps have a constant 1/ITERATIONS? adn multiply?
            return;
        }
        float temp = a;

        a = pow(a, 2) - pow(b, 2) + ainit;
        b = 2 * temp * b + binit;
    }

    *(triple) = 0;

}

void render(SDL_Renderer* p_renderer, float max_x, float max_y, float mid_x, float mid_y)
{
    //Each pixel is scale units apart (cartesian units/pixel)
    float scale_x = 2 * max_x / WIDTH;
    float scale_y = 2 * max_y / HEIGHT;

    //The cartesian point being rendered
    float real;
    float imaginary;

    Uint8 triple = 0;

    for(int pixel_x = 0; pixel_x < WIDTH; pixel_x++)
    {
        real = pixel_x * scale_x - max_x + mid_x;
        for(int pixel_y = 0; pixel_y < HEIGHT; pixel_y++)
        {
            imaginary = pixel_y * scale_y - max_y + mid_y;
            escape(real, imaginary, &(triple));
            SDL_SetRenderDrawColor(p_renderer, triple, 0, 0, 0xFF);
            SDL_RenderDrawPoint(p_renderer, pixel_x, pixel_y);
        }
    }
}

int main()
{
    SDL_Window* p_window = NULL;
    SDL_Renderer* p_renderer = NULL;

    Backend backend = init_backend();

    p_window = backend.p_window;
    p_renderer = backend.p_renderer;



    if(p_window == NULL || p_renderer == NULL) printf("error!");
    
    SDL_Event e;

    int quit = 0;

    //Plotting
    render(p_renderer, 3, 3, 0, 0);
    SDL_RenderPresent(p_renderer);



    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT) quit = 1;
        }

    }

    printf("ending");
    del_backend(p_window, p_renderer);

}