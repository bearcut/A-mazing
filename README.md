#  A-Mazing Pathfinding Visualizer

A high-performance, real-time maze generation and pathfinding visualizer built in C. This project demonstrates procedural generation algorithms, graph traversal, and responsive UI design using **Raylib** and **POSIX Multithreading**.

---

##  Features

###  Maze Generation Algorithms
- **Depth-First Search (DFS)**: Stack-based recursive generation with backtracking
- **Binary Tree**: Fast, biased generation algorithm
- **Sidewinder**: Horizontal bias generation with efficient carving

###  Pathfinding Solvers
- **A***: Heuristic-based optimal pathfinding
- **Breadth-First Search (BFS)**: Guaranteed shortest path
- **Depth-First Search (DFS)**: Memory-efficient exploration
- **Dijkstra's Algorithm**: Uniform cost search with optimality

###  GUI Features
- **Interactive Setup Screen**: Configure maze dimensions (5-201 cells)
- **Real-Time Visualization**: Watch algorithms in action with color-coded cells
  - Green: Start position
  - Red: Goal position
  - Blue: Explored cells
  - Yellow: Final path
  - Dark Gray: Walls
- **Speed Control**: Adjustable solver delay (0-∞ ms) for visualization pacing
- **Live Benchmark**: Compare all four solvers on the same maze
- **Resizable Window**: Dynamic UI adapting to window size (minimum 800×600)
- **Abort Functionality**: Stop solver mid-execution

###  Technical Features
- **Multithreading**: Solver runs on separate thread to prevent UI blocking
  - Uses POSIX pthread for cross-platform compatibility
  - Thread-safe grid access with mutex locks
- **Optimized Data Structures**: 
  - Min-heap implementation for A* and Dijkstra
  - Queue for BFS
  - Stack for DFS
- **Dynamic Memory Management**: Proper allocation/deallocation of maze and solver structures
- **Performance Benchmarking**: Run all four algorithms and compare execution times

## Project Structure

```
A-mazing/
├── src/
│   ├── main.c          # Main application loop and GUI handling
│   ├── generators.c    # Maze generation algorithms
│   ├── solvers.c       # Pathfinding algorithm implementations
│   ├── maze.c          # Maze data structure and utilities
│   └── display.c       # Rendering functions using Raylib
├── include/
│   ├── maze.h
│   ├── algorithms.h
│   ├── display.h
├── bin/
│   └── a_mazing        # Compiled executable
├── build/              # Build artifacts
├── Makefile
└── README.md
```

## Installation & Build

### Prerequisites
- GCC compiler
- Raylib library (graphics rendering)
- POSIX threading (pthread)
- Make

### Building(Linux)

```bash
git clone https://github.com/bearcut/A-mazing.git
cd A-mazing
make
./bin/a_mazing
```
### For Windows

Download from [releases](https://github.com/bearcut/A-mazing/releases/tag/v1.0.1).

## How to Use

### Setup Screen
1. Set desired **Width** and **Height** for the maze (odd numbers recommended)
2. Use +/- buttons to adjust dimensions or type directly
3. Click one of three generation buttons:
   - **Generate: Depth-First Search** - Produces varied, winding mazes
   - **Generate: Binary Tree** - Fast generation with bias
   - **Generate: Sidewinder** - Efficient with horizontal passages

### Solver Screen
1. Watch maze visualization on the left
2. Use sidebar controls to:
   - **Solve buttons**: Run each pathfinding algorithm
   - **Speed Delay**: Adjust visualization speed (0-50+ ms per step)
   - **Refresh**: Regenerate maze with same parameters
   - **Run Benchmark**: Execute all solvers in sequence and compare times
   - **Abort Solving**: Stop current solver execution
3. Click **Back** to return to setup and generate a new maze

## Algorithm Details

### Maze Generation
- **Time Complexity**: O(width × height) for all algorithms
- **Space Complexity**: O(width × height) for grid storage
- All algorithms guarantee a valid, solvable maze

### Pathfinding
- **A***: O((width × height) × log(width × height)) - Most efficient for single goal
- **BFS**: O(width × height) - Optimal, explores level-by-level
- **DFS**: O(width × height) - Memory efficient, may find longer paths
- **Dijkstra**: O((width × height) × log(width × height)) - Uniform cost guarantee

## Key Implementation Details

### Thread Safety
- Global `gridMutex` protects concurrent maze grid access
- Solver runs in detached thread without blocking main render loop
- Proper synchronization prevents race conditions in visualization

### Performance Optimizations
- Min-heap for efficient priority queue operations in A* and Dijkstra
- Thread delay functions (usleep on Linux, Sleep on Windows)
- Dynamic cell sizing based on window dimensions
- Minimal mutex lock duration for grid updates

### Memory Management
- Proper allocation/deallocation of maze structures
- Heap cleanup after solver completion
- No memory leaks in benchmark operations

## Controls

| Action | Method |
|--------|--------|
| Adjust maze size | Type in textbox or use +/- buttons |
| Select width/height field | Click textbox |
| Generate maze | Click generation algorithm button |
| Start solver | Click "Solve: [Algorithm]" button |
| Stop solver | Click "ABORT SOLVING" button |
| Adjust speed | Type in delay box or use +/- buttons |
| Run benchmark | Click "Run Benchmark" |
| Return to setup | Click "Back" |

## Platform Support

- **Linux/Unix**: Full support with pthread
- **Windows**: Compatible with POSIX wrapper or native Windows threading (code includes preprocessor directives)
- **macOS**: Full support

## Future Enhancements

- Additional maze algorithms (Prim's, Kruskal's)
- More pathfinding algorithms (Bidirectional BFS, Greedy Best-First)
- Export maze to file formats
- Statistics panel with detailed metrics
- Multiple goal pathfinding
- 3D maze rendering

## License

This project is open source and available under the MIT License.

