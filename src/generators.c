#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algorithms.h"
#include "maze.h"


int isValid(maze* m, int x, int y) {
    return (x > 0 && x < m->width - 1 && y > 0 && y < m->height - 1);
}


int getIndex(maze* m, int x, int y) {
    return (y * m->width) + x;
}


void generateMazeDFS(maze* m) {
    srand(time(NULL));


    for (int i = 0; i < m->width * m->height; i++) {
        m->grid[i] = wallCell; 
    }

    
    int maxStackSize = m->width * m->height;
    Position* stack = (Position*)malloc(maxStackSize * sizeof(Position));
    int top = -1; 

    
    int startX = 1;
    int startY = 1;
    
   
    m->grid[getIndex(m, startX, startY)] = emptyCell;
    stack[++top] = (Position){startX, startY};

    
    int dx[] = {0, 0, 2, -2};
    int dy[] = {-2, 2, 0, 0};

    
    while (top >= 0) {
        
        int cx = stack[top].x;
        int cy = stack[top].y;

        
        int validNeighbors[4];
        int numValid = 0;

        
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            
            if (isValid(m, nx, ny) && m->grid[getIndex(m, nx, ny)] == wallCell) {
                validNeighbors[numValid++] = i; 
            }
        }

        if (numValid > 0) {
            
            int randDirIndex = validNeighbors[rand() % numValid];
            
            int chosenDx = dx[randDirIndex];
            int chosenDy = dy[randDirIndex];

            
            int nx = cx + chosenDx;
            int ny = cy + chosenDy;

            
            int wallX = cx + chosenDx / 2;
            int wallY = cy + chosenDy / 2;

            
            m->grid[getIndex(m, wallX, wallY)] = emptyCell;
            m->grid[getIndex(m, nx, ny)] = emptyCell;

            
            stack[++top] = (Position){nx, ny};
        } else {
            
            top--;
        }
    }

    
    free(stack);

    
    m->start.x = 1; m->start.y = 1; m->start.Type = startCell;
    m->goal.x = m->width - 2; m->goal.y = m->height - 2; m->goal.Type = goalCell;

    m->grid[getIndex(m, 1, 1)] = startCell;
    m->grid[getIndex(m, m->width - 2, m->height - 2)] = goalCell;
}