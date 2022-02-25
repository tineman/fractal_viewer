'''
Fractal Things :)
'''

import pygame

DIMENSION = (500, 500)
#DIMENSION is the dimension of the screen
SCALE = 2
#SCALE is the absolute value of the largest point on the x axis
ITERATIONS = 20
#ITERATIONS is the number of times the program checks before concluding the thing is stable


def escape(a: float, b: float, iterations: int) -> tuple:
    '''
    Return a 3-tuple giving the RGB value the pixel at coordinate (a, bi) should be coloured
    using iterations iterations
    '''
    #i = 0 is the first iteration of the function
    ainit = a
    binit = b

    for i in range(iterations):
        if(a > 2 or b > 2):
            colour = (200 * ((iterations - i)/iterations))
            return(colour, 0, 0)
        temp = a
        a = (a ** 2) - (b ** 2) + ainit
        b = 2 * temp * b + binit

    return (255, 255, 255)

if __name__ == "__main__":

    #pygame.init()

    '''
    for x in range(DIMENSION[0]):
        for y in range(DIMENSION[1]):

    '''

    while True:
        x = float(input("Please input an x coordinate\n"))
        y = float(input("Please input a y coordinate\n"))
        iter = int(input("Please input the number of iterations you want\n"))
        print(escape(x, y, iter))

    #pygame.event.get() what?

    screen = pygame.display.set_mode(DIMENSION)
    screen.fill((80, 200, 200))

    pygame.display.flip()
