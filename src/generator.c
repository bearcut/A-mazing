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

    // 3. Setup the Stack
    // The maximum possible depth is the total area of the maze.
    int maxStackSize = m->width * m->height;
    Position* stack = (Position*)malloc(maxStackSize * sizeof(Position));
    int top = -1; // Stack pointer

    // 4. Choose a starting point (must be odd coordinates to leave walls intact)
    int startX = 1;
    int startY = 1;
    
    // Set start cell to empty and push to stack
    m->grid[getIndex(m, startX, startY)] = emptyCell;
    stack[++top] = (Position){startX, startY};

    // Direction arrays for North, South, East, West (moving 2 steps at a time!)
    int dx[] = {0, 0, 2, -2};
    int dy[] = {-2, 2, 0, 0};

    // 5. The Core Carving Loop
    while (top >= 0) {
        // Peek at the current cell on top of the stack
        int cx = stack[top].x;
        int cy = stack[top].y;

        // Array to keep track of valid neighbors we can carve into
        int validNeighbors[4];
        int numValid = 0;

        // Check all 4 directions
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            // If neighbor is within bounds AND is currently a wall
            if (isValid(m, nx, ny) && m->grid[getIndex(m, nx, ny)] == wallCell) {
                validNeighbors[numValid++] = i; // Store the direction index
            }
        }

        if (numValid > 0) {
            // We have valid neighbors! 
            // Pick a random valid direction
            int randDirIndex = validNeighbors[rand() % numValid];
            
            int chosenDx = dx[randDirIndex];
            int chosenDy = dy[randDirIndex];

            // Calculate new neighbor coordinates
            int nx = cx + chosenDx;
            int ny = cy + chosenDy;

            // Calculate the wall in between the current cell and the neighbor
            int wallX = cx + chosenDx / 2;
            int wallY = cy + chosenDy / 2;

            // Knock down the wall and clear the neighbor cell
            m->grid[getIndex(m, wallX, wallY)] = emptyCell;
            m->grid[getIndex(m, nx, ny)] = emptyCell;

            // Push the new neighbor onto the stack to move there
            stack[++top] = (Position){nx, ny};
        } else {
            // Dead end. Pop the stack to backtrack.
            top--;
        }
    }

    // 6. Cleanup memory and set start/goal markers
    free(stack);

    // Set arbitrary start and goal markers for your solvers later
    m->start.x = 1; m->start.y = 1; m->start.Type = startCell;
    m->goal.x = m->width - 2; m->goal.y = m->height - 2; m->goal.Type = goalCell;
    
    // Visually mark them on the grid
    m->grid[getIndex(m, 1, 1)] = startCell;
    m->grid[getIndex(m, m->width - 2, m->height - 2)] = goalCell;
}

