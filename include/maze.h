#ifndef MAZE_H
#define MAZE_H
#include<stdbool.h>
  

typedef enum{
    emptyCell = 0,
    wallCell = 1,
    startCell = 2,
    goalCell = 3
} cellType;


typedef struct
{
    int x;
    int y;
    cellType Type;
} cell;


typedef struct
{
    int height;
    int width;
    cell goal;
    cell start;
    cellType *grid;
} maze;
//Maze Object

typedef struct {
    int x, y;          //manhattan distance ke liye
    int index;         // The 1D index in the maze->grid array
    
    float g_cost;      // start se cost
    float h_cost;      // current node se end tak ki cost
    float f_cost;      // f=g+h
    
    int parent_index;  // index of cell jaha se we came(-1 if none)
    
    bool is_open;      // Is this node currently in the Priority Queue?
    bool is_closed;    // Have we already fully evaluated this node?
    bool is_path;      // Is this node part of the final path?
} AStarNode;

typedef struct {
    int x;
    int y;
} Position;


maze* allocMaze(int height,int width); 
void freeMaze(maze* maze); 
cell* getCell(maze* maze,int x,int y);
cellType getType(cell* cell);
void setCell(maze* maze,cell* cell,cellType);
#endif