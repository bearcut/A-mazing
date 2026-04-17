#include "raylib.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "maze.h"
#include "algorithms.h"
#include "display.h"

// --- Globals ---
pthread_mutex_t gridMutex = PTHREAD_MUTEX_INITIALIZER;
int solverDelayMS = 10;
bool isSolving = false;

// --- State Machine Enum ---
typedef enum {
    SCREEN_SETUP,
    SCREEN_SOLVER
} AppScreen;

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
    
    // Center text roughly
    int textWidth = MeasureText(text, 20);
    DrawText(text, rec.x + (rec.width - textWidth) / 2, rec.y + (rec.height - 20) / 2, 20, BLACK);
    return clicked;
}

// Simple Benchmark Runner
// Runs algorithms instantly (0 delay) on the current maze and prints results
char benchmarkResults[256] = "";
void runBenchmark(maze* m) {
    int originalDelay = solverDelayMS;
    solverDelayMS = 0; // Force 0 delay for instant solving
    
    double start, end;
    
    resetMazePaths(m);
    start = GetTime();
    solveAstar((void*)m); // Call directly, not as a thread, to block and measure
    end = GetTime();
    double timeAstar = end - start;

    resetMazePaths(m);
    start = GetTime();
    solveBFS((void*)m); 
    end = GetTime();
    double timeBFS = end - start;
    
    // Format results to display on screen
    snprintf(benchmarkResults, sizeof(benchmarkResults), "Benchmark: A* = %.4f s | BFS = %.4f s", timeAstar, timeBFS);
    
    resetMazePaths(m);
    solverDelayMS = originalDelay; // Restore user delay
}


int main() {
    int screenWidth = 1000;
    int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "A-Mazing Pathfinding");
    SetTargetFPS(60);

    AppScreen currentScreen = SCREEN_SETUP;
    maze* myMaze = NULL;
    pthread_t solverThread;
    
    // Default Dimensions (Must be odd numbers for maze generation)
    int targetWidth = 41;
    int targetHeight = 41;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // SCREEN 1: SETUP & GENERATION
        if (currentScreen == SCREEN_SETUP) {
            DrawText("MAZE SETUP", screenWidth/2 - MeasureText("MAZE SETUP", 40)/2, 100, 40, DARKGRAY);

            // Dimension Controls
            DrawText(TextFormat("Width: %d", targetWidth), 350, 250, 20, BLACK);
            if (DrawButton((Rectangle){460, 240, 40, 40}, "-")) { if (targetWidth > 5) targetWidth -= 2; }
            if (DrawButton((Rectangle){510, 240, 40, 40}, "+")) { if (targetWidth < 201) targetWidth += 2; }

            DrawText(TextFormat("Height: %d", targetHeight), 350, 310, 20, BLACK);
            if (DrawButton((Rectangle){460, 300, 40, 40}, "-")) { if (targetHeight > 5) targetHeight -= 2; }
            if (DrawButton((Rectangle){510, 300, 40, 40}, "+")) { if (targetHeight < 201) targetHeight += 2; }

            // Generator Buttons
            if (DrawButton((Rectangle){300, 400, 400, 50}, "Generate: Depth-First Search")) {
                if (myMaze) freeMaze(myMaze);
                myMaze = allocMaze(targetHeight, targetWidth);
                generateMazeDFS(myMaze);
                benchmarkResults[0] = '\0'; // Clear old benchmark text
                currentScreen = SCREEN_SOLVER;
            }
            if (DrawButton((Rectangle){300, 470, 400, 50}, "Generate: Binary Tree")) {
                if (myMaze) freeMaze(myMaze);
                myMaze = allocMaze(targetHeight, targetWidth);
                generateMazeBinaryTree(myMaze);
                benchmarkResults[0] = '\0';
                currentScreen = SCREEN_SOLVER;
            }
            if (DrawButton((Rectangle){300, 540, 400, 50}, "Generate: Sidewinder")) {
                if (myMaze) freeMaze(myMaze);
                myMaze = allocMaze(targetHeight, targetWidth);
                generateMazeSidewinder(myMaze);
                benchmarkResults[0] = '\0';
                currentScreen = SCREEN_SOLVER;
            }
        }

        // SCREEN 2: VISUALIZATION & SOLVING
        else if (currentScreen == SCREEN_SOLVER) {
            
            // Top UI Panel Background
            DrawRectangle(0, 0, screenWidth, 80, LIGHTGRAY);
            DrawLine(0, 80, screenWidth, 80, BLACK);

            // Back Button
            if (!isSolving && DrawButton((Rectangle){10, 20, 80, 40}, "Back")) {
                currentScreen = SCREEN_SETUP;
            }

            if (!isSolving) {
                // Solvers
                if (DrawButton((Rectangle){110, 20, 100, 40}, "A*")) {
                    resetMazePaths(myMaze);
                    pthread_create(&solverThread, NULL, solveAstar, (void*)myMaze);
                    pthread_detach(solverThread);
                }
                if (DrawButton((Rectangle){220, 20, 100, 40}, "BFS")) {
                    resetMazePaths(myMaze);
                    pthread_create(&solverThread, NULL, solveBFS, (void*)myMaze);
                    pthread_detach(solverThread);
                }

                // Benchmark
                if (DrawButton((Rectangle){340, 20, 120, 40}, "Benchmark")) {
                    runBenchmark(myMaze);
                }

                // Delay config
                DrawText(TextFormat("Delay: %d ms", solverDelayMS), 500, 30, 20, DARKGRAY);
                if (DrawButton((Rectangle){640, 20, 40, 40}, "-")) { if(solverDelayMS > 0) solverDelayMS -= 5; }
                if (DrawButton((Rectangle){690, 20, 40, 40}, "+")) { solverDelayMS += 5; }
            } else {
                DrawText("Solving in progress... Please wait.", 110, 30, 20, RED);
            }

            // Draw Benchmark Results (if any)
            if (benchmarkResults[0] != '\0' && !isSolving) {
                DrawText(benchmarkResults, 10, 90, 20, BLUE);
            }

            // Draw the maze
            if (myMaze) {
                drawMazeRaylib(myMaze, screenWidth, screenHeight);
            }
        }

        EndDrawing();
    }

    if (myMaze) freeMaze(myMaze);
    CloseWindow();
    return 0;
}