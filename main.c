/*

gcc -Wall -Wextra -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 main.c

App that allows the user to zoom in and pan around the mandelbrot set fractal.

    -

TODO

    pan, zoom
    pan optimization (only new areas)
    get better algorithm

    - Test the detail given by larger numbers of iterations (what if you use doubles? is there a way to get arbitray precision?)
    - Test with small numbers, see if you can increase amt zoomable (what is the max zoomability given current precision?)
    - Test for bugs
        - Potential bug - when rendering(3, 3), there are non-black areas that are beyond the square bound by (-2, -2) and (2, 2) - OH because I only check if a, b >2, not if a, b < -2
    - Colouring
    - GIF generator

    - Equation editor so you can reuse the code to see different sets (e.g. burning ship fractal)

    - Optimize (diff. plotting algoriths?) [definitly]
    - Read more about associated Julia sets?
    

*/

#include <stdio.h>
#include <math.h> 
#include <SDL2/SDL.h>

#define WIDTH 480
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
    RENDERS the mandelbrot set such that ...
    Warning: max_x:max_y :: WIDTH:HEIGHT, otherwise the fractal will be stretched/compressed

    \param p_renderer The renderer on which the function renders
    \param max_x The largest cartesian x-value on the screen
    \param max_y The largest cartesian y-value on the screen
    \param mid_x The cartesian x-value at the center of the screen
    \param mid_y The cartesian y-value at the center of the screen
*/
void render(SDL_Renderer* p_renderer, float max_x, float max_y, float mid_x, float mid_y);



/*
    PANS the current camera when the left mouse button is pressed. The function
    pans the current X and Y coordinates of the screen at a one to one ratio to 
    the distance the mouse moves. The function exits when the mouse button is
    released.

    \param x_init The mouse's x_coordinate (pixel point) at the time the mouse is pressed
    \param y_init The mouse's y_coordinate (pixel point) at the time the mouse is pressed
    \param max_x The magnitude of the postive x-axis
    \param max_y The amgnitude of the positive y-axis
    \param p_xmid The pointer to the current x-midpoint (unit: Cartesian points)
    \param p_ymid The pointer to the current y_midpoint (unit: Cartesian points)
*/
void pan(SDL_Renderer* p_renderer, int x_init, int y_init, float max_x, float max_y, float* p_xmid, float* p_ymid);



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

    SDL_RenderPresent(p_renderer);

}



void pan(SDL_Renderer* p_renderer, int x_init, int y_init, float max_x, float max_y, float* p_xmid, float* p_ymid)
{
    SDL_Event e;

    int quit = 0;

    const int refreshrate = 100; //once every 82 miliseconds ~= 12 FPS

    Uint32 time_init = SDL_GetTicks();

    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {

            if(e.type == SDL_MOUSEBUTTONUP) quit = 1;

            else if(e.type == SDL_MOUSEMOTION && (e.motion.timestamp - time_init > refreshrate))
            {
                time_init = e.motion.timestamp;

                *(p_xmid) = *(p_xmid) - ((e.motion.x - x_init) * ((2 * max_x)/WIDTH));
                *(p_ymid) = *(p_ymid) + ((y_init - e.motion.y) * ((2 * max_y)/HEIGHT));
    
                x_init = e.motion.x;
                y_init = e.motion.y;

                render(p_renderer, max_x, max_y, *(p_xmid), *(p_ymid));
                
            }
            
        }

    }
}



int main()
{
    //Initializing window and renderer
    SDL_Window* p_window = NULL;
    SDL_Renderer* p_renderer = NULL;

    Backend backend = init_backend();

    p_window = backend.p_window;
    p_renderer = backend.p_renderer;

    //----------------------------------//

    SDL_Event e;

    int quit = 0;

    //----------------------------------//

    float xmid = 0;
    float ymid = 0;
    float max_x = 3;
    float max_y = 3;

    //------ Initial Plotting -----------//

    render(p_renderer, max_x, max_y, xmid, ymid);

    //------ Main Loop -------//

    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT) quit = 1;

            else if(e.type == SDL_MOUSEBUTTONDOWN) 
            {
                pan(p_renderer, e.button.x, e.button.y, max_x, max_y, &(xmid), &(ymid));
            }

            else if(e.type == SDL_KEYDOWN)
            {
                if(e.key.keysym.sym == SDLK_d)
                {
                    max_x *= 0.75;
                    max_y *= 0.75;
                    render(p_renderer, max_x, max_y, xmid, ymid);
                }

                else if(e.key.keysym.sym == SDLK_f)
                {
                    max_x *= 1.25;
                    max_y *= 1.25;
                    render(p_renderer, max_x, max_y, xmid, ymid);
                }
            }
        } 

    }

    printf("ending\n");
    del_backend(p_window, p_renderer);

}
