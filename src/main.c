#include "raylib.h"
#include <pthread.h>
#include <stdio.h>
#include "maze.h"
#include "algorithms.h"
#include "display.h"

bool isSolving = false;
int solverDelayMS = 0;
pthread_mutex_t gridMutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    
    int width,height;
    
    
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