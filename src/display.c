#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algorithms.h"
#include "maze.h"



void printMazeConsole(maze* m) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            int type = m->grid[(y * m->width) + x];
            if (type == wallCell) printf("██"); // Wall
            else if (type == startCell) printf(" S"); 
            else if (type == goalCell) printf(" G"); 
            else if (type == pathCell) printf(" *"); 
            else printf("  "); 
        }
        printf("\n");
    }
}