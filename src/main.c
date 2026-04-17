#include "raylib.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maze.h"
#include "algorithms.h"
#include "display.h"

// --- Globals ---
pthread_mutex_t gridMutex = PTHREAD_MUTEX_INITIALIZER;
int solverDelayMS = 10;
bool isSolving = false;
bool abortSolver = false; // Add the definition here

// --- Enums ---
typedef enum { SCREEN_SETUP, SCREEN_SOLVER } AppScreen;
typedef enum { GEN_DFS, GEN_BINTREE, GEN_SIDEWINDER } GeneratorType;

// --- Helper Functions ---
bool DrawButton(Rectangle rec, const char* text) {
    bool clicked = false;
    Vector2 mouse = GetMousePosition();
    Color btnColor = LIGHTGRAY;
    if (CheckCollisionPointRec(mouse, rec)) {
        btnColor = GRAY;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) clicked = true;
    }
    DrawRectangleRec(rec, btnColor);
    DrawRectangleLinesEx(rec, 2, BLACK);
    int textWidth = MeasureText(text, 20);
    DrawText(text, rec.x + (rec.width - textWidth) / 2, rec.y + (rec.height - 20) / 2, 20, BLACK);
    return clicked;
}

bool DrawNumericTextBox(Rectangle rec, char* text, int maxLen, bool isActive) {
    bool clicked = false;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), rec)) clicked = true;
    }
    DrawRectangleRec(rec, isActive ? LIGHTGRAY : RAYWHITE);
    DrawRectangleLinesEx(rec, 2, isActive ? BLUE : DARKGRAY);

    if (isActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= '0' && key <= '9' && (int)strlen(text) < maxLen - 1) {
                int len = strlen(text);
                text[len] = (char)key;
                text[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(text);
            if (len > 0) text[len - 1] = '\0';
        }
    }
    int textWidth = MeasureText(text, 20);
    DrawText(text, rec.x + (rec.width - textWidth) / 2, rec.y + (rec.height - 20) / 2, 20, BLACK);
    return clicked;
}

// --- Benchmark Runner ---
char benchmarkResults[512] = "";
void runBenchmark(maze* m) {
    int originalDelay = solverDelayMS;
    solverDelayMS = 0; 
    double start, end;
    
    resetMazePaths(m); start = GetTime(); solveAstar((void*)m); end = GetTime(); double timeAstar = end - start;
    resetMazePaths(m); start = GetTime(); solveBFS((void*)m); end = GetTime(); double timeBFS = end - start;
    resetMazePaths(m); start = GetTime(); solveDFS((void*)m); end = GetTime(); double timeDFS = end - start;
    resetMazePaths(m); start = GetTime(); solveDijkstra((void*)m); end = GetTime(); double timeDijkstra = end - start;
    
    snprintf(benchmarkResults, sizeof(benchmarkResults), 
             "Results:\n\nA*:       %.4f s\nBFS:      %.4f s\nDFS:      %.4f s\nDijkstra: %.4f s", 
             timeAstar, timeBFS, timeDFS, timeDijkstra);
    
    resetMazePaths(m);
    solverDelayMS = originalDelay; 
}


int main() {
    int screenWidth = 1000;
    int screenHeight = 800;
    
    // MAKE THE WINDOW RESIZABLE
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "A-Mazing Pathfinding");
    SetTargetFPS(60);
    SetWindowMinSize(800, 600); // Prevent window from getting too small

    AppScreen currentScreen = SCREEN_SETUP;
    maze* myMaze = NULL;
    pthread_t solverThread;
    GeneratorType currentGen = GEN_DFS; 

    char widthStr[8] = "41";
    char heightStr[8] = "41";
    char delayStr[8] = "10";
    int activeBox = 0; 

    int targetWidth = 41;
    int targetHeight = 41;

    int sidebarWidth = 260;
    
    while (!WindowShouldClose()) {
        // CONSTANTLY UPDATE DIMENSIONS FOR RESIZING
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        int mazeAreaWidth = screenWidth - sidebarWidth;
        int sbX = mazeAreaWidth; 

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) activeBox = 0; 

        targetWidth = atoi(widthStr);
        targetHeight = atoi(heightStr);
        solverDelayMS = atoi(delayStr);

        // ==========================================
        // SCREEN 1: SETUP & GENERATION
        // ==========================================
        if (currentScreen == SCREEN_SETUP) {
            DrawText("MAZE SETUP", screenWidth/2 - MeasureText("MAZE SETUP", 40)/2, 100, 40, DARKGRAY);

            // Center the setup UI based on new width
            int centerOffsetX = screenWidth / 2 - 120;

            DrawText("Width", centerOffsetX, 220, 20, DARKGRAY);
            if (DrawNumericTextBox((Rectangle){(float)centerOffsetX, 250, 100, 40}, widthStr, 8, activeBox == 1)) activeBox = 1;
            if (DrawButton((Rectangle){(float)centerOffsetX, 300, 45, 40}, "-")) { 
                if (targetWidth > 5) targetWidth -= 2; 
                sprintf(widthStr, "%d", targetWidth);
            }
            if (DrawButton((Rectangle){(float)centerOffsetX + 55, 300, 45, 40}, "+")) { 
                if (targetWidth < 201) targetWidth += 2; 
                sprintf(widthStr, "%d", targetWidth);
            }

            DrawText("Height", centerOffsetX + 180, 220, 20, DARKGRAY);
            if (DrawNumericTextBox((Rectangle){(float)centerOffsetX + 180, 250, 100, 40}, heightStr, 8, activeBox == 2)) activeBox = 2;
            if (DrawButton((Rectangle){(float)centerOffsetX + 180, 300, 45, 40}, "-")) { 
                if (targetHeight > 5) targetHeight -= 2; 
                sprintf(heightStr, "%d", targetHeight);
            }
            if (DrawButton((Rectangle){(float)centerOffsetX + 235, 300, 45, 40}, "+")) { 
                if (targetHeight < 201) targetHeight += 2; 
                sprintf(heightStr, "%d", targetHeight);
            }

            if (targetWidth % 2 == 0) targetWidth++;
            if (targetHeight % 2 == 0) targetHeight++;

            if (DrawButton((Rectangle){(float)screenWidth/2 - 200, 400, 400, 50}, "Generate: Depth-First Search")) {
                if (myMaze) freeMaze(myMaze); myMaze = allocMaze(targetHeight, targetWidth); generateMazeDFS(myMaze);
                currentGen = GEN_DFS; benchmarkResults[0] = '\0'; currentScreen = SCREEN_SOLVER;
            }
            if (DrawButton((Rectangle){(float)screenWidth/2 - 200, 470, 400, 50}, "Generate: Binary Tree")) {
                if (myMaze) freeMaze(myMaze); myMaze = allocMaze(targetHeight, targetWidth); generateMazeBinaryTree(myMaze);
                currentGen = GEN_BINTREE; benchmarkResults[0] = '\0'; currentScreen = SCREEN_SOLVER;
            }
            if (DrawButton((Rectangle){(float)screenWidth/2 - 200, 540, 400, 50}, "Generate: Sidewinder")) {
                if (myMaze) freeMaze(myMaze); myMaze = allocMaze(targetHeight, targetWidth); generateMazeSidewinder(myMaze);
                currentGen = GEN_SIDEWINDER; benchmarkResults[0] = '\0'; currentScreen = SCREEN_SOLVER;
            }
        }

        // ==========================================
        // SCREEN 2: VISUALIZATION & SOLVING
        // ==========================================
        else if (currentScreen == SCREEN_SOLVER) {
            
            if (myMaze) drawMazeRaylib(myMaze, mazeAreaWidth, screenHeight); 

            DrawRectangle(sbX, 0, sidebarWidth, screenHeight, LIGHTGRAY);
            DrawLine(sbX, 0, sbX, screenHeight, DARKGRAY);

            if (!isSolving) {
                if (DrawButton((Rectangle){(float)sbX + 20, 20, 105, 40}, "Back")) currentScreen = SCREEN_SETUP;
                if (DrawButton((Rectangle){(float)sbX + 135, 20, 105, 40}, "Refresh")) {
                    if (myMaze) freeMaze(myMaze);
                    myMaze = allocMaze(targetHeight, targetWidth);
                    if (currentGen == GEN_DFS) generateMazeDFS(myMaze);
                    else if (currentGen == GEN_BINTREE) generateMazeBinaryTree(myMaze);
                    else if (currentGen == GEN_SIDEWINDER) generateMazeSidewinder(myMaze);
                    benchmarkResults[0] = '\0';
                }

                DrawText("Algorithms", sbX + 20, 100, 20, DARKGRAY);

                if (DrawButton((Rectangle){(float)sbX + 20, 130, 220, 40}, "Solve: A*")) {
                    resetMazePaths(myMaze); pthread_create(&solverThread, NULL, solveAstar, (void*)myMaze); pthread_detach(solverThread);
                }
                if (DrawButton((Rectangle){(float)sbX + 20, 180, 220, 40}, "Solve: BFS")) {
                    resetMazePaths(myMaze); pthread_create(&solverThread, NULL, solveBFS, (void*)myMaze); pthread_detach(solverThread);
                }
                if (DrawButton((Rectangle){(float)sbX + 20, 230, 220, 40}, "Solve: DFS")) {
                    resetMazePaths(myMaze); pthread_create(&solverThread, NULL, solveDFS, (void*)myMaze); pthread_detach(solverThread);
                }
                if (DrawButton((Rectangle){(float)sbX + 20, 280, 220, 40}, "Solve: Dijkstra")) {
                    resetMazePaths(myMaze); pthread_create(&solverThread, NULL, solveDijkstra, (void*)myMaze); pthread_detach(solverThread);
                }

                DrawText("Speed Delay (ms)", sbX + 20, 350, 20, DARKGRAY);
                if (DrawNumericTextBox((Rectangle){(float)sbX + 20, 380, 220, 40}, delayStr, 8, activeBox == 3)) activeBox = 3;

                if (DrawButton((Rectangle){(float)sbX + 20, 430, 105, 40}, "-")) { 
                    if(solverDelayMS > 0) solverDelayMS -= 5; 
                    sprintf(delayStr, "%d", solverDelayMS);
                }
                if (DrawButton((Rectangle){(float)sbX + 135, 430, 105, 40}, "+")) { 
                    solverDelayMS += 5; 
                    sprintf(delayStr, "%d", solverDelayMS);
                }

                DrawLine(sbX + 20, 500, sbX + 240, 500, GRAY);
                if (DrawButton((Rectangle){(float)sbX + 20, 530, 220, 40}, "Run Benchmark")) runBenchmark(myMaze);
                
                if (benchmarkResults[0] != '\0') DrawText(benchmarkResults, sbX + 20, 600, 20, BLACK);
                
            } else {
                // DRAW THE ABORT BUTTON WHEN SOLVING
                if (DrawButton((Rectangle){(float)sbX + 20, 400, 220, 50}, "ABORT SOLVING")) {
                    abortSolver = true; // Signal the thread to stop immediately
                }
            }
        }
        EndDrawing();
    }

    if (myMaze) freeMaze(myMaze);
    CloseWindow();
    return 0;
}