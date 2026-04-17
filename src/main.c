#include <stdio.h>
#include "maze.h"
#include "algorithms.h"
#include "display.h"



int main() {
    
    int width,height;
    scanf("%d %d",&width,&height);

    printf("Allocating maze...\n");
    maze* myMaze = allocMaze(height, width);

    printf("Generating DFS maze...\n");
    generateMazeDFS(myMaze);

    printf("Solving maze with A*...\n");
    solveDijkstra(myMaze);

    printf("Resulting Maze:\n");
    printMazeConsole(myMaze);
   
    freeMaze(myMaze);
    return 0;
}