/*

gcc -Wall -Wextra -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 main.c

debugging (counting the number of for loops)
_DEBUG_ITERATIONS
_DEBUG_SUBRENDER

gcc -Wall -Wextra -D_DEBUG_SUBRENDER -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 main.c

App that allows the user to zoom in and pan around the mandelbrot set fractal.



TODO

    - rectangle dividing algorithm
    - Fix pan and scale, optimise using array copying in one dimension
    - linked list of panels to check (hard code a testing suite)
    - period checking
    - Equation editor so you can reuse the code to see different sets (e.g. burning ship fractal) (use function pointers)
    - GIF generator
    - REPL

*/

#include <stdio.h>
#include <math.h> 
#include <SDL2/SDL.h>
#include "helper.h"

//----------------------------------//
#ifdef _DEBUG_ITERATIONS
int num_iterations = 0;
#endif //_DEBUG_ITERATIONS


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

            #ifdef _DEBUG_ITERATIONS
            num_iterations += i;
            #endif //_DEBUG_ITERATIONS

            return i;
        }
        float temp = query.real;

        query.real = pow(query.real, 2) - pow(query.imag, 2) + init.real;
        query.imag = 2 * temp * query.imag + init.imag;
    }

    #ifdef _DEBUG_ITERATIONS
    num_iterations += ITERATIONS;
    #endif //_DEBUG_ITERATIONS

    return 0;

}

//debug subrender using a new function which maps the i, jth entry of the input array to a copy of hte correct array

void subrender(Uint8 color[HEIGHT][WIDTH], Pixel top_left, Pixel bottom_right, Pixel line, Coord scale, int eval_line, Coord max, Coord mid) //before calling this you need to escape the perimeter first //call with height - 1...
{



    //To be used on small arrays
    #ifdef _DEBUG_SUBRENDER
    printf("Subrender called on (%d, %d) and (%d, %d)\n", top_left.x, top_left.y, bottom_right.x, bottom_right.y);
    printf("The current state fo the array is:\n");
    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            printf("%d, ", color[i][j]);
        }
        printf("\n");
    }
    #endif //#ifdef _DEBUG_SUBRENDER



    if((top_left.x == bottom_right.x) && (top_left.y = bottom_right.y)) return;

    if(eval_line)
    {
        Coord query;

        if(line.y == (bottom_right.y - 1)) //vertical parititon
        {
            query.real = bottom_right.x * scale.real - max.real + mid.real;
            for(int i = top_left.y + 1; i < bottom_right.y; i++)
            {
                query.imag = i * scale.imag - max.imag + mid.imag;
                color[bottom_right.x][i] = (Uint8) escape(query);
            }
        }
        else //horizontal partition
        {
            query.imag = bottom_right.y * scale.imag - max.imag + mid.imag;
            for(int i = top_left.x + 1; i < bottom_right.x; i++)
            {
                query.real = i * scale.real - max.real + mid.real;
                color[i][bottom_right.y] = (Uint8) escape(query);
            }
        }
        //call escape and write to color
        //check with bottom corner point it uses to define a line
    }

    Uint8 color_check = color[top_left.x][top_left.y];
    int succeed = 0;

    //checks the top row
    for(int i = top_left.x + 1; (i <= bottom_right.x && succeed); i++) if(color_check != color[i][top_left.y]) succeed = 1;
    //checks the right column
    for(int i = top_left.y + 1; (i <= bottom_right.y && succeed); i++) if(color_check != color[bottom_right.x][i]) succeed = 1;
    //Checks the bottom row
    for(int i = top_left.x; (i < bottom_right.x && succeed); i++) if(color_check != color[i][bottom_right.y]) succeed = 1;
    //checks the left column
    for(int i = top_left.y + 1; (i < bottom_right.y && succeed); i++) if(color_check != color[top_left.x][i]) succeed = 1;



    //DEBUG
    #ifdef _DEBUG_SUBRENDER
    if(succeed) printf("The subarray defined by (%d, %d) and (%d, %d) was filled in completely with the color corresponding to %d\n", top_left.x, top_left.y, bottom_right.x, bottom_right.y, color_check);
    #endif //#ifdef _DEBUG_SUBRENDER



    if(succeed)
    {
        for(int i = top_left.x + 1; i < bottom_right.x; i++)
        {
            for(int j = top_left.y + 1; j < bottom_right.y; j++)
            {
                color[i][j] = color_check;
            }
        }
        return;
    }



    if((bottom_right.x - top_left.x) > (bottom_right.y - top_left.y))
    {
        //divide horizontally
        int partition_line_x = floor((bottom_right.x - top_left.x + 1) / 2);
        Pixel new_bottom = bottom_right;
        new_bottom.x = partition_line_x;
        Pixel new_top = top_left;
        new_top.x = partition_line_x;
        Pixel partition = new_bottom;
        partition.y = new_bottom.y - 1;

        subrender(color, top_left, new_bottom, partition, scale, 0, max, mid);
        return subrender(color, new_top, bottom_right, partition, scale, 1, max, mid);
    }
    else
    {
        //divide vertically
        int partition_line_y = floor((bottom_right.y - top_left.y + 1) / 2);
        Pixel new_bottom = bottom_right;
        new_bottom.y = partition_line_y;
        Pixel new_top = top_left;
        new_top.y = partition_line_y;
        Pixel partition = new_bottom;
        partition.y = new_bottom.x - 1;

        subrender(color, top_left, new_bottom, partition, scale, 0, max, mid);
        return subrender(color, new_top, bottom_right, partition, scale, 1, max, mid);
    }

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

    #ifdef _DEBUG_ITERATIONS
    printf("Rendering the image at %f + %fi at a magnitude of %fX i took %d for loops! If you still had to pay for computer time, you'd be one sad panda!\n", mid.real, mid.imag, max.real, num_iterations);
    num_iterations = 0;
    #endif //_DEBUG_ITERATIONS

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
