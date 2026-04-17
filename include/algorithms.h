#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "maze.h"

//generators
void generateMazeBinaryTree(maze* m);
void generateMazeSidewinder(maze* m);
void generateMazeDFS(maze* m);

//solvers
void solveAstar(maze* m);
void solveDijkstra(maze* m);
void solveBFS(maze* m);
void solveDFS(maze* m);

#endif 