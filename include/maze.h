#ifndef MAZE_H
#define MAZE_H
  

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
    int x;
    int y;
} Position;


maze* allocMaze(int height,int width); 
void freeMaze(maze* maze); 
cell* getCell(maze* maze,int x,int y);
cellType getType(cell* cell);
void setCell(maze* maze,cell* cell,cellType);
#endif