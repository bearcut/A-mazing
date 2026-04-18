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

void convertToBraidMaze(maze* m, double extraWallRemovalChance) {
    srand(time(NULL));

    // Directional offsets for North, South, East, West
    int dx[] = {0, 0, 1, -1};
    int dy[] = {-1, 1, 0, 0};

    for (int y = 1; y < m->height - 1; y += 2) {
        for (int x = 1; x < m->width - 1; x += 2) {
            
            // Only evaluate empty path cells
            if (m->grid[getIndex(m, x, y)] == wallCell) continue;

            int wallCount = 0;
            int validWalls[4][2]; // Stores coordinates of walls we can safely remove
            int numValidWalls = 0;

            // Check all 4 surrounding directions
            for (int i = 0; i < 4; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];

                if (m->grid[getIndex(m, nx, ny)] == wallCell) {
                    wallCount++;
                    
                    int pastWallX = nx + dx[i];
                    int pastWallY = ny + dy[i];
                    
                    if (isValid(m, pastWallX, pastWallY)) {
                        validWalls[numValidWalls][0] = nx;
                        validWalls[numValidWalls][1] = ny;
                        numValidWalls++;
                    }
                }
            }

            if (wallCount == 3 && numValidWalls > 0) {
                // Pick a random valid wall and knock it down
                int randIndex = rand() % numValidWalls;
                int wx = validWalls[randIndex][0];
                int wy = validWalls[randIndex][1];
                m->grid[getIndex(m, wx, wy)] = emptyCell;
            }
        }
    }

    if (extraWallRemovalChance > 0.0) {
        for (int y = 1; y < m->height - 1; y++) {
            for (int x = 1; x < m->width - 1; x++) {
                
                if (m->grid[getIndex(m, x, y)] == wallCell) {
                    // Check if the wall separates two empty spaces horizontally or vertically
                    bool separatesHorizontally = (m->grid[getIndex(m, x-1, y)] == emptyCell && 
                                                  m->grid[getIndex(m, x+1, y)] == emptyCell);
                                                  
                    bool separatesVertically =   (m->grid[getIndex(m, x, y-1)] == emptyCell && 
                                                  m->grid[getIndex(m, x, y+1)] == emptyCell);

                    if (separatesHorizontally || separatesVertically) {
                        double chance = (double)rand() / RAND_MAX;
                        if (chance < extraWallRemovalChance) {
                            m->grid[getIndex(m, x, y)] = emptyCell;
                        }
                    }
                }
            }
        }
    }
}