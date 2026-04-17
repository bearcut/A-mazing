#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

maze* allocMaze(int height,int width){
    maze* newMaze = (maze*)malloc(sizeof(maze));
    cellType* allocGrid = (cellType*)calloc(height*width,sizeof(cellType));
    newMaze->height = height;
    newMaze->width = width;
    newMaze->grid = allocGrid;
    return newMaze; 
}

void freeMaze(maze* maze){
    if (maze){
        free(maze->grid);
        free(maze);
    }
}

void resetMazePaths(maze* m) {
    pthread_mutex_lock(&gridMutex);
    for (int i = 0; i < m->width * m->height; i++) {
        if (m->grid[i] == pathCell || m->grid[i] == exploredCell) {
            m->grid[i] = emptyCell;
        }
    }
    pthread_mutex_unlock(&gridMutex);
}