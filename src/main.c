#include <stdio.h>
#include "maze.h"
#include "algorithms.h"
#include "display.h"



int main() {
    // DFS works best with odd dimensions to ensure walls align properly
    int width,height;
    scanf("%d %d",&width,&height);

    printf("Allocating maze...\n");
    maze* myMaze = allocMaze(height, width);

    printf("Generating DFS maze...\n");
    generateMazeDFS(myMaze);

    printf("Resulting Maze:\n");
    printMazeConsole(myMaze);

    freeMaze(myMaze);
    return 0;
}