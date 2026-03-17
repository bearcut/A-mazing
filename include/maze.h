#ifndef MAZE_H
#define MAZE_H
  

typedef enum{
    emptyCell = 0,
    wallCell = 1,
    startCell = 2,
    goalCell = 3
} cellType;
//cellType Data Type

typedef struct
{
    int x;
    int y;
    cellType Type;
} cell;
//cell position and type in a single object

typedef struct
{
    int height;
    int width;
    cell goal;
    cell start;
    cellType *grid;
} maze;
//Maze Object

maze* allocMaze(int height,int width); //Allocate Memory for maze
void freeMaze(maze* maze); // free memory
cell* getCell(maze* maze,int x,int y);// Get cell object from coordinated
cellType getType(cell* cell);// Get the cellType from cell
void setCell(maze* maze,cell* cell,cellType);//set celltype
#endif