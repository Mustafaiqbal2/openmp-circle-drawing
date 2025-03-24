# OpenMP Circle Visualization

This project demonstrates the use of OpenMP for parallel processing by implementing circle drawing using Taylor series approximations of sine and cosine functions. It visualizes a circle while comparing the performance of serial and parallel implementations.

## Overview

The program creates a circle using parametric equations:
- x(t) = r cos(t) + j
- y(t) = r sin(t) + k

Where:
- (j, k) is the origin point
- r is the radius
- t ranges from 0 to 360 degrees

The implementation uses two levels of parallelism:
1. Computing (x,y) points around the circle in parallel
2. Calculating Taylor series terms for sine and cosine functions in parallel

## Dependencies

- C++ compiler with OpenMP support
- SDL2 graphics library
- CMake (optional, for building)

## Installation

### Ubuntu/Debian
```bash
# Install dependencies
sudo apt update
sudo apt install g++ libsdl2-dev

# Clone the repository
git clone https://github.com/yourusername/openmp-circle-drawing.git
cd openmp-circle-drawing

# Compile
g++ -fopenmp -O3 circle_drawing.cpp -o circle_drawing -lSDL2
```

### macOS (with Homebrew)
```bash
# Install dependencies
brew install llvm libomp sdl2

# Compile using LLVM (for OpenMP support)
/opt/homebrew/opt/llvm/bin/clang++ -fopenmp -O3 circle_drawing.cpp -o circle_drawing -lSDL2
```

### Windows (with MinGW)
```bash
# Assuming MinGW and SDL2 are installed
g++ -fopenmp -O3 circle_drawing.cpp -o circle_drawing -lSDL2
```

## Usage

Run the compiled executable:
```bash
./circle_drawing
```

The program will:
1. Display OpenMP version and available threads
2. Run performance benchmarks for serial and parallel implementations
3. Draw a smooth circle using SDL2
4. Show performance comparisons in the console

## Implementation Details

### Taylor Series

The program implements Taylor series expansions for sine and cosine:

- sin(x) = x - x³/3! + x⁵/5! - x⁷/7! + ...
- cos(x) = 1 - x²/2! + x⁴/4! - x⁶/6! + ...

### Parallelization Strategy

The code uses OpenMP at two levels:

1. **Outer Parallelization**: 
   - Distributes the computation of points along the circle
   - Each thread computes a subset of angles

2. **Inner Parallelization**: 
   - Distributes computation of individual Taylor series terms
   - Uses reduction to combine partial results

### Optimization Techniques

- Normalization of angles for numerical stability
- Thread synchronization with barriers and atomic operations
- Reduction operations for efficient summation of Taylor series
- Dynamic thread allocation based on system capabilities

## Performance Analysis

The program provides detailed performance metrics:
- Comparison between serial and parallel implementations
- Scaling with different thread counts
- Separate measurements for standard library vs. Taylor series implementations

## Code Structure

- **taylor_sine/taylor_cosine**: Implement Taylor series approximations
- **compute_circle_points**: Calculates circle points in parallel
- **draw_circle**: Renders the circle using SDL2
- **main**: Handles initialization, benchmarking, and cleanup

## Academic Context

This project demonstrates principles of parallel computing including:
- Task decomposition and load balancing
- Synchronization mechanisms
- Performance scaling with thread count
- Tradeoffs between computational approaches


## Acknowledgments

- The SDL2 development team for the graphics library
- OpenMP Architecture Review Board for the parallel processing framework