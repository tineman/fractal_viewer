/*

gcc -Wall -Wextra -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 main.c

debugging (counting the number of for loops)

gcc -Wall -Wextra -D_MAGPIEDEBUGFLAG -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 main.c

App that allows the user to zoom in and pan around the mandelbrot set fractal.



TODO

    - Fix pan and scale
    - linked list of panels to check (hard code a testing suite)
    - period checking
    - rectangle dividing algorithm
    - Equation editor so you can reuse the code to see different sets (e.g. burning ship fractal) (use function pointers)
    - GIF generator
    - arbitrary precision
    - REPL

*/

#include <stdio.h>
#include <math.h> 
#include <SDL2/SDL.h>
#include "helper.h"

//----------------------------------//
#ifdef _MAGPIEDEBUGFLAG
int num_iterations = 0;
#endif //_MAGPIEDEBUGFLAG


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

            #ifdef _MAGPIEDEBUGFLAG
            num_iterations += i;
            #endif //_MAGPIEDEBUGFLAG

            return i;
        }
        float temp = query.real;

        query.real = pow(query.real, 2) - pow(query.imag, 2) + init.real;
        query.imag = 2 * temp * query.imag + init.imag;
    }

    #ifdef _MAGPIEDEBUGFLAG
    num_iterations += ITERATIONS;
    #endif //_MAGPIEDEBUGFLAG

    return 0;

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
            Uint8 triple = (Uint8) (255 * ( (double) (escape(point))/ITERATIONS) );

            SDL_SetRenderDrawColor(p_renderer, triple, 0, 0, 0xFF);
            SDL_RenderDrawPoint(p_renderer, pixel_x, pixel_y);
            
        }

    }

    SDL_RenderPresent(p_renderer);

    #ifdef _MAGPIEDEBUGFLAG
    printf("Rendering the image at %f + %fi at a magnitude of %fX i took %d for loops! If you still had to pay for computer time, you'd be one sad panda!\n", mid.real, mid.imag, max.real, num_iterations);
    num_iterations = 0;
    #endif //_MAGPIEDEBUGFLAG

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

    //Variables for controlling the main loop

    SDL_Event e;

    int quit = 0;

    //----------------------------------//

    //Variables defining the current camera's region

    Coord mid;
    mid.real = 0;
    mid.imag = 0;

    Coord max;
    max.real = 3;
    max.imag = 3;

    //------ Initial Plotting -----------//

    render(p_renderer, max, mid);

    //------ Main Loop -------//

    
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT) quit = 1;

            /*
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
            */
        } 

    }

    printf("ending\n");
    del_backend(p_window, p_renderer);

}
