/*

App that allows the user explore the Mandlebrot fractal and save gifs.

Public Domain.

Tian Yu

*/

#include <SDL2/SDL.h>
#include "helper.h"
#include "gifenc.h"

//----------------------------------//

//SDL components
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
Backend init_backend()
{
    SDL_Init(SDL_INIT_VIDEO);

    Backend backend;

    backend.p_window = SDL_CreateWindow("Fractal Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    backend.p_renderer = SDL_CreateRenderer(backend.p_window, -1, SDL_RENDERER_ACCELERATED);

    return backend;
}


/*
    FREES p_window and p_renderer

    \param p_window The pointer to where the window will be stored
    \param p_renderer The pointer to where the renderer will be stored
*/
void del_backend(SDL_Window* p_window, SDL_Renderer* p_renderer)
{
    SDL_RenderClear(p_renderer);
    SDL_DestroyWindow(p_window);
    SDL_Quit();
}

//----------------------------------//

// Print the options available
void print_options()
{
    printf("\n======= OPTIONS =======\n"
    "-1) Quit\n"
    "1) View current cordinates\n"
    "2) Go to coordinates\n"
    "3) Pan with mouse\n"
    "\n======= GIF CREATION OPTIONS =======\n"
    "4) Check snapshot\n"
    "5) Add current frame as snapshot\n"
    "6) Delete snapshot\n"
    "7) Save gif\n"
    "8) Display options\n");
}

//----------------------------------//

//Functions for testing if a point is in the mandelbrot set

/*
    RETURNS the number of iterations it takes for query to escape. Return 0 if query does not escape (arbitrary decision to make colouring easier)

    \param query - the coordinate in question
*/
int escape(Coord query)
{

    Coord init;

    init.real = query.real;
    init.imag = query.imag;

    for(float i = 1; i <= pow(2, PALETTE_DEPTH); i++) 
    {
        if(query.real > 2 || query.imag > 2)
        {
            return i;
        }
        float temp = query.real;

        query.real = pow(query.real, 2) - pow(query.imag, 2) + init.real;
        query.imag = 2 * temp * query.imag + init.imag;
    }

    return 0;

}

/*
    RENDERS the mandelbrot set
    Warning: max.real:max.imag :: WIDTH:HEIGHT, otherwise the fractal will be stretched/compressed

    \param p_renderer The renderer on which the function renders
    \param max The largest coordinate on the screen
    \param mid The coordinate at the centre of the screen
*/
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
            Uint8 triple = (Uint8) (255 * ( (double) (escape(point))/pow(2, PALETTE_DEPTH)) );

            SDL_SetRenderDrawColor(p_renderer, triple, 0, 0, 0xFF);
            SDL_RenderDrawPoint(p_renderer, pixel_x, pixel_y);
            
        }

    }

    SDL_RenderPresent(p_renderer);

}

/*
    ADD the frame specified to the gif
    Warning: max.real:max.imag :: WIDTH:HEIGHT, otherwise the fractal will be stretched/compressed

    \param gif The gif the frame is added to
    \param max The largest coordinate on the screen
    \param mid The coordinate at the centre of the screen
    \param sidelength The sidelength of the gif
    \param mili_duration The duration of the frame in miliseconds
*/
void gif_render(ge_GIF* gif, Coord max, Coord mid, int sidelength, int mili_duration)
{
    //Each pixel is scale units apart (cartesian units/pixel)
    Coord scale;
    scale.real = 2 * max.real / sidelength;
    scale.imag = 2 * max.imag / sidelength;

    //The cartesian point being rendered
    Coord point;

    for(int pixel_x = 0; pixel_x < sidelength; pixel_x++)
    {
        point.real = pixel_x * scale.real - max.real + mid.real;
        
        for(int pixel_y = 0; pixel_y < sidelength; pixel_y++)
        {
            point.imag = pixel_y * scale.imag - max.imag + mid.imag;

            gif->frame[(pixel_y * sidelength) + pixel_x] = escape(point);

        }

    }

    ge_add_frame(gif, mili_duration);

}



/*
    Renders the gif specified by the snapshots in root

    \param filename The filename of the gif
    \param sidelength The sidelength of the gif
    \param root The linked list of snapshots to be rendered
*/void save_gif(char* filename, int sidelength, Panel_Node* root)
{
    if(root == NULL)
    {
        printf("No snapshots in the current gif. Returning to main menu\n");
        return;
    }

    uint8_t palette[(int) pow(2, PALETTE_DEPTH) * 3];

    for(int i = 0; i < pow(2, PALETTE_DEPTH); i++)
    {
        palette[3 * i] = i * 255 / (pow(2, PALETTE_DEPTH) - 1);
        palette[3 * i + 1] = 0;
        palette[3 * i + 2] = 0;
    }
    
    ge_GIF *gif = ge_new_gif(
        filename,
        sidelength, sidelength,
        palette,
        PALETTE_DEPTH,
        -1,
        0
    );

    Panel_Node* next_panel = root->next;

    int snapshot_index = 0;
    int mili_duration = 1;

    while(next_panel != NULL)
    {

        Coord frame_max = {.real = root->max.real, .imag = root->max.imag};
        Coord frame_mid = {.real = root->mid.real, .imag = root->mid.imag};

        int numframes = FRAMERATE * root->duration;

        //Calculating the change in mid and max with every frame
        Coord delta_mid = {.real = (next_panel->mid.real - root->mid.real) / numframes, .imag = (next_panel->mid.imag - root->mid.imag) / numframes};
        Coord ddelta_max = {.real = ((next_panel->max.real - next_panel->mid.real) - (root->max.real - root->mid.real)) / numframes,
                        .imag = ((next_panel->max.imag - next_panel->mid.imag) - (root->max.imag - root->mid.imag)) / numframes};


        mili_duration = (int) ((1.0 / FRAMERATE) * 1000);


        for(int i = 0; i <= FRAMERATE * root->duration; i++)
        {
            gif_render(gif, frame_max, frame_mid, sidelength, mili_duration);
            //Debugging
            //printf("Snapshot %d, Frame %d/%d at (%Lf, %Lf) with max (%Lf, %Lf) and duration %d\n", snapshot_index + 1, i, FRAMERATE * root->duration, frame_mid.real, frame_mid.imag, frame_max.real, frame_max.imag, mili_duration);

            frame_max.real += ddelta_max.real + delta_mid.real;
            frame_max.imag += ddelta_max.imag + delta_mid.imag;

            frame_mid.real += delta_mid.real;
            frame_mid.imag += delta_mid.imag;

        }

        snapshot_index++;

        root = next_panel;
        next_panel = root->next;

    }
    gif_render(gif, root->max, root->mid, sidelength, mili_duration);

    ge_close_gif(gif);
    printf("%s created\n", filename);
}


/*
    PANS the current camera when the left mouse button is pressed. The function
    pans the current X and Y coordinates of the screen at a one to one ratio to 
    the distance the mouse moves. The function exits when the mouse button is
    released.

    \param init The mouse's pixel coordinates at the time the mouse is pressed
    \param max The magnitude of the area being rendered
    \param p_mid The pointer to the current midpoint
*/
void pan(SDL_Renderer* p_renderer, Pixel init, Coord max, Coord* p_mid)
{
    SDL_Event e;

    int quit = 0;

    const int refreshrate = 80; //once every 82 miliseconds ~= 12 FPS

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

    //Variables for saving a gif, index starting at 1

    Panel_Node* root = NULL;
    int num_snapshots = 0;
    int panel_index = 1;

    char name[128];

    //----------------------------------//

    //Variables defining the current camera's region

    Coord mid;
    mid.real = 0;
    mid.imag = 0;

    Coord max;
    max.real = 3;
    max.imag = 3;

    //----------------------------------//

    //Initializing window and renderer
    SDL_Window* p_window = NULL;
    SDL_Renderer* p_renderer = NULL;

    Backend backend = init_backend();

    p_window = backend.p_window;
    p_renderer = backend.p_renderer;

    render(p_renderer, max, mid);

    //------ Main Loop -------//
   
    printf(" ______ _____            _____ _______       _       _____      __  __ ____\n"
           "|  ____|  __ \\     /\\   / ____|__   __|/\\   | |     / ____|    |  \\/  |  _ \\\n"
           "| |__  | |__) |   /  \\ | |       | |  /  \\  | |    | (___      | \\  / | |_) |\n"
           "|  __| |  _  /   / /\\ \\| |       | | / /\\ \\ | |     \\___ \\     | |\\/| |  _ < \n"
           "| |    | | \\ \\  / ____ \\ |____   | |/ ____ \\| |____ ____) | _ _| |  | | |_) |\n"
           "|_|    |_|  \\_\\/_/    \\_\\_____|  |_/_/    \\_\\______|_____(_|_|_)_|  |_|____/ \n");
    printf("\n=======================================================================================\n\n");
    printf("A lightweight Mandelbrot fractal viewer and gif saver.\n");

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
                printf("Ending\n");
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
            
                printf("Entering panning mode. Press Q to exit.\n");
                quit = 0;
                while(!quit)
                {
                    while(SDL_PollEvent(&e) != 0)
                    {
                        if(e.type == SDL_QUIT)
                        {
                            printf("ending\n");
                            del_backend(p_window, p_renderer);
                            return 0;
                        }
                        else if(e.type == SDL_MOUSEBUTTONDOWN) 
                        {
                            Pixel init;
                            init.x = e.button.x;
                            init.y = e.button.y;
                            pan(p_renderer, init, max, &(mid));
                        }

                        else if(e.type == SDL_KEYDOWN)
                        {
                            if(e.key.keysym.sym == SDLK_d)
                            {
                                max.real *= 0.75;
                                max.imag *= 0.75;
                                render(p_renderer, max, mid);
                            }

                            else if(e.key.keysym.sym == SDLK_f)
                            {
                                max.real *= 1.25;
                                max.imag *= 1.25;
                                render(p_renderer, max, mid);
                            }

                            else if(e.key.keysym.sym == SDLK_q)
                            {
                                quit = 1;
                                printf("Exitting panning mode\n");
                            }
                        }
                    } 

                }

                break;

            case 4: //check gif information
                printInfo(root);
                break;

            case 5: //add snapshot (no error checking)
                
                if(num_snapshots == 0) printf("Adding your snapshot to index 1 since the gif is empty.\n");
                else
                {
                    printf("Please input the index (1-%d) of your new snapshot\n", num_snapshots + 1);
                    scanf("%127s", input);
                    getchar();
                    if(atoi(input) <= 0)
                    {
                        printf("Invalid input, returning to main menu\n");
                        break;
                    }
                    panel_index = atoi(input);
                }

                printf("Please input the duration of your snapshot. This is the number of time, in seconds, it will take to reach the next snapshot.\n");
                scanf("%127s", input);
                getchar();
                if(atoi(input) <= 0)
                {
                    printf("Invalid input, returning to main menu\n");
                    break;
                }
                if(num_snapshots == 0) root = newPanel(max, mid, atoi(input));

                else root = addPanel(root, newPanel(max, mid, atoi(input)), panel_index);

                printf("Panel added\n");
                num_snapshots++;
                break;

            case 6: //delete snapshot

                if(num_snapshots == 0)
                {
                    printf("There are currently no frames in your gif\n");
                    break;
                }

                printf("Please input the index (1-%d) of the snapshot you want to delete\n", num_snapshots);
                scanf("%127s", input);
                getchar();
                if(atoi(input) < 1 || atoi(input) > num_snapshots)
                {
                    printf("Invalid input, returning to main menu\n");
                    break;
                }

                root = deletePanel(root, atoi(input));

                printf("Panel deleted\n");
                num_snapshots--;
                break;

            case 7: //save gif
                printf("Please input a name for the gif\n");
                scanf("%127s", name);
                getchar();

                printf("Please input a side length for the gif. Recommended size of 480\n");
                scanf("%127s", input);
                getchar();

                if(atoi(input) <= 0)
                {
                    printf("Invalid input, returning to main menu\n");
                    break;
                }

                printf("Creating %s. This may take a while.\n", name);

                save_gif(name, atoi(input), root);

                //add status bar
                break;

            case 8: //print options
                print_options();
                break;
        }

    }
    

}
