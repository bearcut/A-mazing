#include <stdlib.h>    
#include <stdbool.h>   
#include <math.h>      
#include <pthread.h>
#include "maze.h"      
#include "algorithms.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

//min heap mein parent ki f value always less than child ki f value

typedef struct {
    AStarNode** elements;  //array of pointer to AstarNode structs
    int size;          //nodes in open list 
    int capacity;      //total cells in maze
} MinHeap;


MinHeap* createHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->elements = (AStarNode**)malloc(capacity * sizeof(AStarNode*));      // this part will initialise our min-heap(open list)
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}


void swapNodes(AStarNode** a, AStarNode** b) {
    AStarNode* temp = *a;                                  //pointer swappping function
    *a = *b;
    *b = temp;
}

// Function to push a node into the heap
void pushHeap(MinHeap* heap, AStarNode* node) {
    int current = heap->size;
    heap->elements[current] = node;                  // this block will put the node to the last of our min-heap
    heap->size++;

    while (current > 0) {
        int parent = (current - 1) / 2;
        if (heap->elements[current]->f_cost < heap->elements[parent]->f_cost) {            //this block will sort the min-heap in such a way so that 
            swapNodes(&heap->elements[current], &heap->elements[parent]);                  //the minimum f-cost node stays at index 0 and the heap remains sorted
            current = parent; 
        } else {
            break;
        }
    }
}

// Pop the best node from the heap 
AStarNode* popHeap(MinHeap* heap) {
    if (heap->size <= 0) return NULL; 

    AStarNode* root = heap->elements[0];
    heap->size--;
    heap->elements[0] = heap->elements[heap->size];

    int current = 0;
    while (true) {
        int left = 2 * current + 1;                                  //this block will pop the lowest f-cost node and sort the min-heap
        int right = 2 * current + 2;          
        int smallest = current;

        if (left < heap->size && heap->elements[left]->f_cost < heap->elements[smallest]->f_cost) {
            smallest = left;
        }
        
        if (right < heap->size && heap->elements[right]->f_cost < heap->elements[smallest]->f_cost) {
            smallest = right;
        }

        if (smallest != current) {
            swapNodes(&heap->elements[current], &heap->elements[smallest]);
            current = smallest; 
        } else {
            break; 
        }
    }
    return root;
}


void freeHeap(MinHeap* heap) {
    if (heap != NULL) {
        free(heap->elements);    // to free up memory
        free(heap);           
    }
}

float get_h_cost(int x1, int y1, int x2, int y2) {                                                          
    return (float)(abs(x1 - x2) + abs(y1 - y2));                  //to calculate manhattan distance
}

void applyDelay() {
    if (solverDelayMS > 0) {
#ifdef _WIN32
        Sleep(solverDelayMS); 
#else
        usleep(solverDelayMS * 1000); 
#endif
    }
}

void* solveAstar(void* arg) {
    isSolving = true;
    abortSolver = false; // Reset abort flag
    maze* m = (maze*)arg;
    int totalCells = m->width * m->height;
    AStarNode* solverGrid = malloc(totalCells * sizeof(AStarNode)); 

    for(int i = 0; i < totalCells; i++) {            
        solverGrid[i].index = i;
        solverGrid[i].x = i % m->width;    
        solverGrid[i].y = i / m->width;    
        solverGrid[i].g_cost = 999999; 
        solverGrid[i].is_open = false;   
        solverGrid[i].is_closed = false;
        solverGrid[i].parent_index = -1;
        solverGrid[i].is_path = false;   
    }

    MinHeap* openList = createHeap(totalCells);
    int startIdx = (m->start.y * m->width) + m->start.x;
    
    solverGrid[startIdx].g_cost = 0;
    solverGrid[startIdx].f_cost = get_h_cost(m->start.x, m->start.y, m->goal.x, m->goal.y);
    solverGrid[startIdx].is_open = true;
    pushHeap(openList, &solverGrid[startIdx]);

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while (openList->size > 0) {
        if (abortSolver) break; // Check for abort

        AStarNode* current = popHeap(openList);
        current->is_open = false;
        current->is_closed = true;

        // Visual update for explored cells
        pthread_mutex_lock(&gridMutex);
        if (m->grid[current->index] != startCell && m->grid[current->index] != goalCell) {
            m->grid[current->index] = exploredCell;
        }
        pthread_mutex_unlock(&gridMutex);
        applyDelay();

        // GOAL CHECK & BACKWARD PATH RECONSTRUCTION
        if (current->x == m->goal.x && current->y == m->goal.y) {
            int currIdx = current->index;
            while (currIdx != -1) {
                if (abortSolver) break; // Check for abort during backtracking
                
                solverGrid[currIdx].is_path = true; 
                
                // Visual update for path drawing
                pthread_mutex_lock(&gridMutex);
                if (m->grid[currIdx] != startCell && m->grid[currIdx] != goalCell) {
                    m->grid[currIdx] = pathCell;
                }
                pthread_mutex_unlock(&gridMutex);
                applyDelay();

                currIdx = solverGrid[currIdx].parent_index; 
            }
            break; 
        }

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
    free(solverGrid);
    isSolving = false;
    abortSolver = false;
    return NULL;
}

void* solveDijkstra(void* arg) {
    isSolving = true;
    abortSolver = false; // Reset abort flag
    maze* m = (maze*)arg;
    int totalCells = m->width * m->height;
    AStarNode* solverGrid = malloc(totalCells * sizeof(AStarNode)); 

    for(int i = 0; i < totalCells; i++) {            
        solverGrid[i].index = i;
        solverGrid[i].x = i % m->width;    
        solverGrid[i].y = i / m->width;    
        solverGrid[i].g_cost = 999999; 
        solverGrid[i].is_open = false;   
        solverGrid[i].is_closed = false;
        solverGrid[i].parent_index = -1;
        solverGrid[i].is_path = false;   
    }

    MinHeap* openList = createHeap(totalCells);
    int startIdx = (m->start.y * m->width) + m->start.x;
    int goalIdx = (m->goal.y * m->width) + m->goal.x;
    
    solverGrid[startIdx].g_cost = 0;
    solverGrid[startIdx].f_cost = 0; // h is always 0 for Dijkstra
    solverGrid[startIdx].is_open = true;
    pushHeap(openList, &solverGrid[startIdx]);

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while (openList->size > 0) {
        if (abortSolver) break; // Check for abort

        AStarNode* current = popHeap(openList);
        current->is_open = false;
        current->is_closed = true;

        // Visual update for explored cells
        pthread_mutex_lock(&gridMutex);
        if (m->grid[current->index] != startCell && m->grid[current->index] != goalCell) {
            m->grid[current->index] = exploredCell;
        }
        pthread_mutex_unlock(&gridMutex);
        applyDelay();

        // GOAL CHECK & BACKWARD PATH RECONSTRUCTION
        if (current->index == goalIdx) {
            int currIdx = current->index;
            while (currIdx != -1) {
                if (abortSolver) break; // Check for abort during backtracking
                
                solverGrid[currIdx].is_path = true; 
                
                // Visual update for path drawing
                pthread_mutex_lock(&gridMutex);
                if (m->grid[currIdx] != startCell && m->grid[currIdx] != goalCell) {
                    m->grid[currIdx] = pathCell;
                }
                pthread_mutex_unlock(&gridMutex);
                applyDelay();
                
                currIdx = solverGrid[currIdx].parent_index; 
            }
            break; 
        }

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
                        solverGrid[neighborIdx].f_cost = tentative_g; 

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
    free(solverGrid);
    isSolving = false;
    abortSolver = false;
    return NULL;
}

void* solveBFS(void* arg) {
    isSolving = true;
    abortSolver = false; // Reset abort flag
    maze* m = (maze*)arg;
    int totalCells = m->width * m->height;
    int* queue = (int*)malloc(totalCells * sizeof(int));
    int* parent = (int*)malloc(totalCells * sizeof(int));
    bool* visited = (bool*)calloc(totalCells, sizeof(bool));

    int head = 0, tail = 0;

    for (int i = 0; i < totalCells; i++) parent[i] = -1;

    int startIdx = (m->start.y * m->width) + m->start.x;
    int goalIdx = (m->goal.y * m->width) + m->goal.x;

    queue[tail++] = startIdx; 
    visited[startIdx] = true;

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while (head < tail) {
        if (abortSolver) break; // Check for abort

        int curr = queue[head++]; 
        
        // Visual update for explored cells
        pthread_mutex_lock(&gridMutex);
        if (m->grid[curr] != startCell && m->grid[curr] != goalCell) {
            m->grid[curr] = exploredCell;
        }
        pthread_mutex_unlock(&gridMutex);
        applyDelay();

        if (curr == goalIdx) break;

        int cx = curr % m->width;
        int cy = curr / m->width;

        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height) {
                int nIdx = ny * m->width + nx;

                if (m->grid[nIdx] != wallCell && !visited[nIdx]) {
                    visited[nIdx] = true;
                    parent[nIdx] = curr;
                    queue[tail++] = nIdx; 
                }
            }
        }
    }

    // Path Reconstruction
    int curr = goalIdx;
    while (parent[curr] != -1 && curr != startIdx) {
        if (abortSolver) break; // Check for abort during backtracking

        curr = parent[curr];
        if (curr != startIdx) {
            pthread_mutex_lock(&gridMutex);
            m->grid[curr] = pathCell;
            pthread_mutex_unlock(&gridMutex);
            applyDelay();
        }
    }

    free(queue);
    free(parent);
    free(visited);
    isSolving = false;
    abortSolver = false;
    return NULL;
}

void* solveDFS(void* arg) {
    isSolving = true;
    abortSolver = false; // Reset abort flag
    maze* m = (maze*)arg;
    int totalCells = m->width * m->height;
    int* stack = (int*)malloc(totalCells * sizeof(int));
    int* parent = (int*)malloc(totalCells * sizeof(int));
    bool* visited = (bool*)calloc(totalCells, sizeof(bool));

    int top = -1;

    for (int i = 0; i < totalCells; i++) parent[i] = -1;

    int startIdx = (m->start.y * m->width) + m->start.x;
    int goalIdx = (m->goal.y * m->width) + m->goal.x;

    stack[++top] = startIdx; 
    visited[startIdx] = true;

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while (top >= 0) {
        if (abortSolver) break; // Check for abort

        int curr = stack[top--]; 
        
        // Visual update for explored cells
        pthread_mutex_lock(&gridMutex);
        if (m->grid[curr] != startCell && m->grid[curr] != goalCell) {
            m->grid[curr] = exploredCell;
        }
        pthread_mutex_unlock(&gridMutex);
        applyDelay();
        
        if (curr == goalIdx) break;

        int cx = curr % m->width;
        int cy = curr / m->width;

        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (nx >= 0 && nx < m->width && ny >= 0 && ny < m->height) {
                int nIdx = ny * m->width + nx;

                if (m->grid[nIdx] != wallCell && !visited[nIdx]) {
                    visited[nIdx] = true;
                    parent[nIdx] = curr;
                    stack[++top] = nIdx; 
                }
            }
        }
    }

    // Path Reconstruction
    int curr = goalIdx;
    while (parent[curr] != -1 && curr != startIdx) {
        if (abortSolver) break; // Check for abort during backtracking
        
        curr = parent[curr];
        if (curr != startIdx) {
            pthread_mutex_lock(&gridMutex);
            m->grid[curr] = pathCell;
            pthread_mutex_unlock(&gridMutex);
            applyDelay();
        }
    }

    free(stack);
    free(parent);
    free(visited);
    isSolving = false;
    abortSolver = false;
    return NULL;
}
