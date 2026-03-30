#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algorithms.h"
#include "maze.h"


// A quick helper function to print the maze to the terminal so you can verify it works
void printMazeConsole(maze* m) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            int type = m->grid[(y * m->width) + x];
            if (type == wallCell) printf("██"); // Wall
            else if (type == startCell) printf(" S"); // Start
            else if (type == goalCell) printf(" G");  // Goal
            else printf("  "); // Empty path
        }
        printf("\n");
    }
}