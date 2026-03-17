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