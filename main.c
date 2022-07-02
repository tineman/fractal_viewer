/*

gcc -Wall -Wextra -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 main.c

App that allows the user to zoom in and pan around the mandelbrot set fractal.

    -

TODO

    pan, zoom
    pan optimization (only new areas)
    get better algorithm

    NO DYNAMIC ITER CHANGE - manual

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
#include "helper.h"



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



int escape(Coord query)
{

    Coord init;

    init.real = query.real;
    init.imag = query.imag;

    for(float i = 0; i < ITERATIONS; i++) 
    {
        if(query.real > 2 || query.imag > 2)
        {
            return i;
        }
        float temp = query.real;

        query.real = pow(query.real, 2) - pow(query.imag, 2) + init.real;
        query.imag = 2 * temp * query.imag + init.imag;
    }

    return ITERATIONS;

}



void render(SDL_Renderer* p_renderer, Coord max, Coord mid)
{
    //Each pixel is scale units apart (cartesian units/pixel)
    Coord scale;
    scale.real = 2 * max.real / WIDTH;
    scale.imag = 2 * max.imag / HEIGHT;

    //The cartesian point being rendered
    Coord point;

    for(int pixel_x = 0; pixel_x < WIDTH; pixel_x++)
    {
        point.real = pixel_x * scale.real - max.real + mid.real;
        for(int pixel_y = 0; pixel_y < HEIGHT; pixel_y++)
        {
            point.imag = pixel_y * scale.imag - max.imag + mid.imag;
            escape(point);

            //

            /*
            SDL_SetRenderDrawColor(p_renderer, triple, 0, 0, 0xFF);
            SDL_RenderDrawPoint(p_renderer, pixel_x, pixel_y);
            */
        }
    }

    //SDL_RenderPresent(p_renderer);

}



void pan(SDL_Renderer* p_renderer, Pixel init, Coord max, Coord* p_mid)
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

                p_mid->real = p_mid->real - ((e.motion.x - init.x) * ((2 * max.real)/WIDTH));
                p_mid->imag = p_mid->imag + ((init.y - e.motion.y) * ((2 * max.imag)/HEIGHT));
    
                init.x = e.motion.x;
                init.y = e.motion.y;

                render(p_renderer, max, *(p_mid));
                
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

    Coord mid;
    mid.real = 0;
    mid.imag = 0;

    Coord max;
    max.real = 3;
    max.imag = 3;

    //------ Initial Plotting -----------//



    //------ Main Loop -------//

    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT) quit = 1;

            else if(e.type == SDL_MOUSEBUTTONDOWN) 
            {
                //pan and render
            }

            else if(e.type == SDL_KEYDOWN)
            {
                if(e.key.keysym.sym == SDLK_d)
                {
                    max.real *= 0.75;
                    max.imag *= 0.75;
                    //zoom and render
                }

                else if(e.key.keysym.sym == SDLK_f)
                {
                    max.real *= 1.25;
                    max.imag *= 1.25;
                    //zoom and render
                }
            }
        } 

    }

    printf("ending\n");
    del_backend(p_window, p_renderer);

}
