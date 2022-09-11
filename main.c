/*

gcc -Wall -Wextra -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 main.c

debugging (counting the number of for loops)
_DEBUG_ITERATIONS
_DEBUG_SUBRENDER

gcc -Wall -Wextra -D_DEBUG_SUBRENDER -I/Library/Frameworks/SDL.framework/Headers -framework SDL2 main.c

App that allows the user to zoom in and pan around the mandelbrot set fractal.

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



void print_options()
{
    printf("OPTIONS\n"
    "-1) Quit\n"
    "1) View current cordinates\n"
    "2) Go to coordinates\n"
    "3) Pan with mouse\n"
    "GIF CREATION OPTIONS\n"
    "4) Check gif and frames\n"
    "5) Edit framerate and gif duration\n"
    "6) Add snapshot\n"
    "7) Delete snapshot\n"
    "8) Save gif\n");
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

    //Initializing window and renderer
    SDL_Window* p_window = NULL;
    SDL_Renderer* p_renderer = NULL;

    Backend backend = init_backend();

    p_window = backend.p_window;
    p_renderer = backend.p_renderer;

    render(p_renderer, max, mid);

    //------ Main Loop -------//

    //Driver
   
    printf(" ______ _____            _____ _______       _       _____      __  __ ____\n"
           "|  ____|  __ \\     /\\   / ____|__   __|/\\   | |     / ____|    |  \\/  |  _ \\\n"
           "| |__  | |__) |   /  \\ | |       | |  /  \\  | |    | (___      | \\  / | |_) |\n"
           "|  __| |  _  /   / /\\ \\| |       | | / /\\ \\ | |     \\___ \\     | |\\/| |  _ < \n"
           "| |    | | \\ \\  / ____ \\ |____   | |/ ____ \\| |____ ____) | _ _| |  | | |_) |\n"
           "|_|    |_|  \\_\\/_/    \\_\\_____|  |_/_/    \\_\\______|_____(_|_|_)_|  |_|____/ \n");

    print_options();

    char input[128];


    while(1)
    {
        SDL_PollEvent(&e);
        
        scanf("%127s", input);
        getchar();

        switch(atoi(input))
        {
            case -1: //quit
                printf("ending\n");
                del_backend(p_window, p_renderer);
                return 0;

            case 0: //invalid input
                printf("Please enter a valid input\n");
                break;

            case 1: //print current information
                printf("The current frame is centered on (%Lf, %Lf) and the top right of the frame is (%Lf, %Lf)\n", mid.real, mid.imag, max.real, max.imag);
                break;

            case 2: //go to coordinates

                printf("Please input a x coordinate for the middle\n");
                scanf("%127s", input);
                getchar();
                mid.real = strtod(input, NULL);
                printf("Please input a y coordinate for the middle\n");
                scanf("%127s", input);
                getchar();
                mid.imag = strtod(input, NULL);
                printf("Please input the x coordinate for the top right corner of the screen\n");
                scanf("%127s", input);
                getchar();
                max.real = strtod(input, NULL);
                printf("Please input the y coordinate for the top right corner of the screen\n");
                scanf("%127s", input);
                getchar();
                max.imag = strtod(input, NULL);

                render(p_renderer, max, mid);
                break;

            case 3: //pan
            


                break;

            case 4: //check gif information
            
                break;

            case 5: //edit gif
            
                break;

            case 6: //add snapshot
            
                break;

            case 7: //delete snapshot
            
                break;

            case 8: //save gif
            
                break;
        }

    }
    

}
