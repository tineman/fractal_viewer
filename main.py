'''
Fractal Things :)

TODO
    - Write down the maths for the pixel -> coordinate scaling
    - Make code more efficient, perhaps write it smth like C?
    - Implement zooming
    - Implement panning

    - Read more about associated Julia sets
    - Equation editor?
'''

import pygame
import pygame.gfxdraw #neccessary for drawing pixels

DIMENSION = (1000, 1000)
#DIMENSION is the dimension of the screen
SCALE = 3
#SCALE is the absolute value of the largest point on the x axis
ITERATIONS = 30
#ITERATIONS is the number of times the program checks before concluding the thing is stable


def escape(a: float, b: float, iterations: int) -> tuple:
    '''
    Return a 3-tuple giving the RGB value the pixel at coordinate (a, bi) should be coloured
    using iterations iterations

    For more information, refer to the binder
    '''

    #i = 0 is the first iteration of the function
    ainit = a
    binit = b

    for i in range(iterations):
        if(a > 2 or b > 2):
            colour = (255 * (i/iterations))
            return(colour, 0, 0)
        temp = a
        a = (a ** 2) - (b ** 2) + ainit
        b = 2 * temp * b + binit

    return (0, 0, 0)

if __name__ == "__main__":

    screen = pygame.display.set_mode(DIMENSION)
    screen.fill((80, 200, 200))

    pygame.init()

    scale_x = SCALE/DIMENSION[0]
    scale_y = SCALE/DIMENSION[1]
    #each pixel is scale units apart

    for pixel_x in range(DIMENSION[0]):
        coord_x = pixel_x * scale_x - SCALE/2
        for pixel_y in range(DIMENSION[1]):
            coord_y = pixel_y * scale_y - SCALE/2
            pygame.gfxdraw.pixel(screen, pixel_x, pixel_y, escape(coord_x, coord_y, ITERATIONS))

    #pygame.event.get() what?

    pygame.display.flip()

    print("running!")

    running = True
    while running:  
    # Did the user click the window close button?
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
