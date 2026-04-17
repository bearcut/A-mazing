#include "raylib.h"
#include <pthread.h>
#include <stdio.h>
#include "maze.h"
#include "algorithms.h"
#include "display.h"


int main() {
    int solverDelayMS = 0;
    int width,height;
    bool isSolving = false;
    
    scanf("%d %d",&width,&height);

    printf("Allocating maze...\n");
    maze* myMaze = allocMaze(height, width);

    printf("Generating DFS maze...\n");
    generateMazeBinaryTree(myMaze);

    printf("Solving maze with A*...\n");
    solveAstar(myMaze);

    printf("Resulting Maze:\n");
    printMazeConsole(myMaze);
   
    freeMaze(myMaze);
    return 0;
}