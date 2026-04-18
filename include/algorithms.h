#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "maze.h"

//generators
void generateMazeBinaryTree(maze* m);
void generateMazeSidewinder(maze* m);
void generateMazeDFS(maze* m);

//solvers
void* solveAstar(void* arg);
void* solveDijkstra(void* arg);
void* solveBFS(void* arg);
void* solveDFS(void* arg);
void convertToBraidMaze(maze* m, double extraWallRemovalChance);

// Benchmark Tool
void runBenchmark(maze* m);

#endif 