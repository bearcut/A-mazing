#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algorithms.h"
#include "maze.h"
#include "display.h"
#include "raylib.h"

void drawMazeRaylib(maze* m, int screenW, int screenH) {
    if (!m || !m->grid) return;

    // Calculate dynamic scaling
    float cellWidth = (float)screenW / m->width;
    float cellHeight = (float)(screenH - 100) / m->height; // Leave room for UI
    float cellSize = (cellWidth < cellHeight) ? cellWidth : cellHeight;

    float offsetX = (screenW - (m->width * cellSize)) / 2.0f;
    float offsetY = 80.0f + ((screenH - 80 - (m->height * cellSize)) / 2.0f);

    // Thread-safe drawing
    pthread_mutex_lock(&gridMutex);
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            int type = m->grid[(y * m->width) + x];
            Color c = RAYWHITE;

            if (type == wallCell) c = DARKGRAY;
            else if (type == startCell) c = GREEN;
            else if (type == goalCell) c = RED;
            else if (type == exploredCell) c = SKYBLUE;
            else if (type == pathCell) c = YELLOW;

            DrawRectangle((int)(offsetX + x * cellSize), (int)(offsetY + y * cellSize), 
                          (int)cellSize, (int)cellSize, c);
        }
    }
    pthread_mutex_unlock(&gridMutex);
}

void printMazeConsole(maze* m) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            int type = m->grid[(y * m->width) + x];
            if (type == wallCell) printf("██"); // Wall
            else if (type == startCell) printf(" S"); 
            else if (type == goalCell) printf(" G"); 
            else if (type == pathCell) printf(" *"); 
            else printf("  "); 
        }
        printf("\n");
    }
}