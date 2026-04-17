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

void generateMazeBinaryTree(maze* m) {
    srand(time(NULL));

    // Fill with walls
    for (int i = 0; i < m->width * m->height; i++) {
        m->grid[i] = wallCell; 
    }

    // Step by 2 to leave space for walls
    for (int y = 1; y < m->height - 1; y += 2) {
        for (int x = 1; x < m->width - 1; x += 2) {
            m->grid[getIndex(m, x, y)] = emptyCell;

            bool at_northern_boundary = (y == 1);
            bool at_western_boundary = (x == 1);

            // Top-left cell has nowhere to carve
            if (at_northern_boundary && at_western_boundary) continue;

            int carve_dir; // 0 for North, 1 for West
            if (at_northern_boundary) carve_dir = 1;
            else if (at_western_boundary) carve_dir = 0;
            else carve_dir = rand() % 2;

            if (carve_dir == 0) {
                m->grid[getIndex(m, x, y - 1)] = emptyCell; // Carve North
            } else {
                m->grid[getIndex(m, x - 1, y)] = emptyCell; // Carve West
            }
        }
    }

    // Set Start and Goal
    m->start.x = 1; m->start.y = 1; m->start.Type = startCell;
    m->goal.x = m->width - 2; m->goal.y = m->height - 2; m->goal.Type = goalCell;
    m->grid[getIndex(m, 1, 1)] = startCell;
    m->grid[getIndex(m, m->width - 2, m->height - 2)] = goalCell;
}

void generateMazeSidewinder(maze* m) {
    srand(time(NULL));

    for (int i = 0; i < m->width * m->height; i++) {
        m->grid[i] = wallCell; 
    }

    for (int y = 1; y < m->height - 1; y += 2) {
        int run_start = 1; // Track the beginning of the current run of cells
        
        for (int x = 1; x < m->width - 1; x += 2) {
            m->grid[getIndex(m, x, y)] = emptyCell;

            bool at_eastern_boundary = (x >= m->width - 3); // Account for edge
            bool at_northern_boundary = (y == 1);
            
            // Close the run if we hit the East edge OR randomly (if not on top row)
            bool should_close_out = at_eastern_boundary || (!at_northern_boundary && (rand() % 2 == 0));

            if (should_close_out) {
                // Carve North from a random cell in the current run
                if (!at_northern_boundary) {
                    int num_cells = (x - run_start) / 2 + 1;
                    int random_offset = (rand() % num_cells) * 2;
                    int rx = run_start + random_offset;
                    m->grid[getIndex(m, rx, y - 1)] = emptyCell;
                }
                run_start = x + 2; // Reset run
            } else {
                // Carve East to continue the run
                m->grid[getIndex(m, x + 1, y)] = emptyCell;
            }
        }
    }

    m->start.x = 1; m->start.y = 1; m->start.Type = startCell;
    m->goal.x = m->width - 2; m->goal.y = m->height - 2; m->goal.Type = goalCell;
    m->grid[getIndex(m, 1, 1)] = startCell;
    m->grid[getIndex(m, m->width - 2, m->height - 2)] = goalCell;
}
