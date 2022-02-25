'''
Fractal Things :)
'''

import pygame

RESOLUTION = (1000, 1000)






if __name__ == "__main__":

    pygame.init()

    #pygame.event.get() what?

    screen = pygame.display.set_mode(RESOLUTION)
    screen.fill((50, 50, 50))

    pygame.display.flip()