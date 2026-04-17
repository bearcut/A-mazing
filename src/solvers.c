#include <stdlib.h>    
#include <stdbool.h>   
#include <math.h>      
#include "maze.h"      
#include "algorithms.h"

//min heap mein parent ki f value always less than child ki f value

typedef struct {
    AStarNode** elements;  //array of pointer to AstarNode structs
    int size;          //nodes in open list 
    int capacity;      //total cells in maze
} MinHeap;

// initialising the heap
MinHeap* createHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->elements = (AStarNode**)malloc(capacity * sizeof(AStarNode*));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

// function to swap pointers 
/* we will push the largest value to the end of the heap and from there we sort that one element such that the 
   condition ki parent node must have the f value less than that of child gets satisfied*/
void swapNodes(AStarNode** a, AStarNode** b) {
    AStarNode* temp = *a;
    *a = *b;
    *b = temp;
}

// Pushing a node into the heap 
void pushHeap(MinHeap* heap, AStarNode* node) {
   
    // Insert at the very end
    int current = heap->size;
    heap->elements[current] = node;
    heap->size++;

    //sorting that one element we inserted at the end
    while (current > 0) {
        int parent = (current - 1) / 2;
        
        // If the current node is better (lower f cost) than its parent, swap them
        if (heap->elements[current]->f_cost < heap->elements[parent]->f_cost) {
            swapNodes(&heap->elements[current], &heap->elements[parent]);
            current = parent; // Move our tracker up to the parent's index
        } else {
            // If the parent is smaller, the rule is satisfied.
            break;
        }
    }
}

// Pop the best node from the heap 
AStarNode* popHeap(MinHeap* heap) {
    if (heap->size <= 0) return NULL; // if heap is empty

    // The best node is always at the top aka index 0 wala element
    AStarNode* root = heap->elements[0];

    // Move the very last node to the top to fill the hole
    heap->size--;
    heap->elements[0] = heap->elements[heap->size];

    // sort that element now which we have filled in the hole
    int current = 0;
    while (true) {
        int left = 2 * current + 1;
        int right = 2 * current + 2;          //imagine a binary tree
        int smallest = current;

        // Checking if left child exists and is smaller than current
        if (left < heap->size && heap->elements[left]->f_cost < heap->elements[smallest]->f_cost) {
            smallest = left;
        }
        
        // Checking if right child exists and is even smaller
        if (right < heap->size && heap->elements[right]->f_cost < heap->elements[smallest]->f_cost) {
            smallest = right;
        }

        // swap with the smallest child and continue the loop to for sorting that specific element
        if (smallest != current) {
            swapNodes(&heap->elements[current], &heap->elements[smallest]);
            current = smallest; // Move our tracker down
        } else {
            // If we are smaller than both children, we have found our rightful place
            break; 
        }
    }

    return root;
}

// cleaning memory after solver finishes
void freeHeap(MinHeap* heap) {
    if (heap != NULL) {
        free(heap->elements); // Free the array of pointers
        free(heap);           // Free the heap struct itself
    }
}

float get_h_cost(int x1, int y1, int x2, int y2) 
{                                                          //manhattan distance for h cost
    return (float)(abs(x1 - x2) + abs(y1 - y2));
}

void solveAstar(maze* m)
{
int totalCells = m->width * m->height;
AStarNode* solverGrid = malloc(totalCells * sizeof(AStarNode)); //allocating another maze of same size as that of maze to work on

for(int i = 0; i < totalCells; i++) {            //loop to initialise each cell 
    solverGrid[i].index = i;
    solverGrid[i].x = i % m->width;    //coords
    solverGrid[i].y = i / m->width;    //coords
    solverGrid[i].g_cost = 999999; //initialise each cell
    solverGrid[i].is_open = false;   
    solverGrid[i].is_closed = false;
    solverGrid[i].parent_index = -1;
    solverGrid[i].is_path = false;   
}
//setting up the start node and the heap
MinHeap* openList = createHeap(totalCells);
    int startIdx = (m->start.y * m->width) + m->start.x;
    
    solverGrid[startIdx].g_cost = 0;
    solverGrid[startIdx].f_cost = get_h_cost(m->start.x, m->start.y, m->goal.x, m->goal.y);
    solverGrid[startIdx].is_open = true;
    pushHeap(openList, &solverGrid[startIdx]);

    //movements
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    //main search loop
    while (openList->size > 0) {
        AStarNode* current = popHeap(openList);
        current->is_open = false;
        current->is_closed = true;

    // GOAL CHECK & PATH RECONSTRUCTION (Backtracking)
        if (current->x == m->goal.x && current->y == m->goal.y) {
            int currIdx = current->index;
            while (currIdx != -1) {
                solverGrid[currIdx].is_path = true; // Mark node as part of final path
                currIdx = solverGrid[currIdx].parent_index; // Walk backward
            }
            break; // Exit the while loop when we are done
        }

    // E. NEIGHBOR EXPLORATION
        for (int i = 0; i < 4; i++) {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];

            if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height) {
                int neighborIdx = (ny * m->width) + nx;

                if (m->grid[neighborIdx] != wallCell && !solverGrid[neighborIdx].is_closed) {
                    float tentative_g = current->g_cost + 1;

                    if (tentative_g < solverGrid[neighborIdx].g_cost) {
                        solverGrid[neighborIdx].parent_index = current->index;
                        solverGrid[neighborIdx].g_cost = tentative_g;
                        solverGrid[neighborIdx].f_cost = tentative_g + get_h_cost(nx, ny, m->goal.x, m->goal.y);

                        if (!solverGrid[neighborIdx].is_open) {
                            solverGrid[neighborIdx].is_open = true;
                            pushHeap(openList, &solverGrid[neighborIdx]);
                        }
                    }
                }
            }
        }
    }
freeHeap(openList);
//reflect the path before freeing
for (int i = 0; i < totalCells; i++) {
        if (solverGrid[i].is_path) {
            // We ensure we don't overwrite the Start or Goal cells 
            // so they still render as 'S' and 'G'
            if (m->grid[i] != startCell && m->grid[i] != goalCell) {
                m->grid[i] = pathCell;
            }
        }
    }
free(solverGrid);
}

