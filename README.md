# C Maze Generator & Solver

A graphical maze generator and solver built from scratch in C using the Raylib library. It uses Depth-First Search (DFS) algorithms to carve out randomized perfect mazes and solve them visually.

## Features
* **Custom Built:** Memory and data structures (like Stacks) are implemented in pure C.
* **DFS Generation:** Uses a randomized recursive backtracker to generate mazes.
* **DFS Solving:** Visually finds the path from the start to the end of the maze.
* **Real-time Graphics:** Watch the algorithms work frame-by-frame using Raylib.

## Requirements
* A C Compiler (GCC, Clang, etc.)
* [Raylib](https://www.raylib.com/) 

## How to Build and Run (doesn't work for now )
1. Clone the repository.
2. Compile using your local C compiler and link Raylib. 
   *(Example for Linux):*
   ```bash
   gcc main.c -o maze -lraylib -lGL -lm -lpthread -ldl -rt -lX11