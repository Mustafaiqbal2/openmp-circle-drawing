#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <SDL2/SDL.h>
#include <iomanip>

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int NUM_POINTS = 3600000; // 10 points per degree for smoothness
const int TAYLOR_TERMS = 10; // Number of terms in Taylor series

// Taylor series computation for sine
double taylor_sine(double x, int terms) {
    // Normalize x to be between -π and π
    while (x > M_PI) x -= 2 * M_PI;
    while (x < -M_PI) x += 2 * M_PI;
    
    double result = 0.0;
    
    #pragma omp parallel reduction(+:result)
    {
        double local_result = 0.0;
        
        #pragma omp for nowait
        for (int i = 1; i <= terms; i += 2) {
            double term_factorial = 1.0;
            for (int j = 1; j <= i; ++j) {
                term_factorial *= j;
            }
            
            double term_power = 1.0;
            for (int j = 1; j <= i; ++j) {
                term_power *= x;
            }
            
            double term = (i % 4 == 1 ? 1.0 : -1.0) * term_power / term_factorial;
            local_result += term;
        }
        
        #pragma omp atomic
        result += local_result;
    }
    
    return result;
}

// Taylor series computation for cosine
double taylor_cosine(double x, int terms) {
    // Normalize x to be between -π and π
    while (x > M_PI) x -= 2 * M_PI;
    while (x < -M_PI) x += 2 * M_PI;
    
    double result = 0.0;
    
    #pragma omp parallel reduction(+:result)
    {
        double local_result = 0.0;
        
        #pragma omp for nowait
        for (int i = 0; i <= terms; i += 2) {
            double term_factorial = 1.0;
            for (int j = 1; j <= i; ++j) {
                term_factorial *= j;
            }
            
            double term_power = 1.0;
            for (int j = 1; j <= i; ++j) {
                term_power *= x;
            }
            
            double term = (i % 4 == 0 ? 1.0 : -1.0) * term_power / term_factorial;
            local_result += term;
        }
        
        #pragma omp atomic
        result += local_result;
    }
    
    return result;
}

// Compute circle points in parallel using Taylor series
void compute_circle_points(int center_x, int center_y, int radius, 
                          std::vector<SDL_Point>& points, bool use_taylor, int num_threads) {
    points.resize(NUM_POINTS);
    
    // Set the number of threads
    omp_set_num_threads(num_threads);
    
    #pragma omp parallel for
    for (int i = 0; i < NUM_POINTS; ++i) {
        double angle = 2.0 * M_PI * i / NUM_POINTS;
        double cos_value, sin_value;
        
        if (use_taylor) {
            cos_value = taylor_cosine(angle, TAYLOR_TERMS);
            sin_value = taylor_sine(angle, TAYLOR_TERMS);
        } else {
            cos_value = cos(angle);
            sin_value = sin(angle);
        }
        
        int x = static_cast<int>(radius * cos_value) + center_x;
        int y = static_cast<int>(radius * sin_value) + center_y;
        
        points[i] = {x, y};
    }
}

// Draw the circle using SDL
void draw_circle(SDL_Renderer* renderer, const std::vector<SDL_Point>& points) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    SDL_RenderDrawPoints(renderer, points.data(), points.size());
}

int main() {

    // You could add this at the beginning of main():
#ifdef _OPENMP
std::cout << "OpenMP Version: " << _OPENMP / 100 << "." << _OPENMP % 100 << std::endl;
std::cout << "Maximum available threads: " << omp_get_max_threads() << std::endl;
#else
std::cout << "OpenMP is not available" << std::endl;
#endif
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Create window
    SDL_Window* window = SDL_CreateWindow("Circle Drawing with OpenMP", 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Circle parameters
    int center_x = WINDOW_WIDTH / 2;
    int center_y = WINDOW_HEIGHT / 2;
    int radius = std::min(WINDOW_WIDTH, WINDOW_HEIGHT) / 3;
    
    std::vector<SDL_Point> points;
    
    // Test with different numbers of threads for performance comparison
    const int max_threads = omp_get_max_threads();
    
    std::cout << "Performance Analysis\n";
    std::cout << "===================\n";
    std::cout << std::left << std::setw(20) << "Implementation" 
              << std::setw(15) << "Threads" 
              << std::setw(15) << "Time (ms)" << std::endl;
    std::cout << "---------------------------------------------------\n";
    
    // Serial implementation using math.h
    double start_time = omp_get_wtime();
    compute_circle_points(center_x, center_y, radius, points, false, 1);
    double end_time = omp_get_wtime();
    std::cout << std::left << std::setw(20) << "Serial (math.h)" 
              << std::setw(15) << 1 
              << std::setw(15) << (end_time - start_time) * 1000.0 << std::endl;
    
    // Serial implementation using Taylor series
    start_time = omp_get_wtime();
    compute_circle_points(center_x, center_y, radius, points, true, 1);
    end_time = omp_get_wtime();
    std::cout << std::left << std::setw(20) << "Serial (Taylor)" 
              << std::setw(15) << 1 
              << std::setw(15) << (end_time - start_time) * 1000.0 << std::endl;
    
    // Parallel implementations using math.h
    for (int num_threads = 2; num_threads <= max_threads; num_threads *= 2) {
        start_time = omp_get_wtime();
        compute_circle_points(center_x, center_y, radius, points, false, num_threads);
        end_time = omp_get_wtime();
        std::cout << std::left << std::setw(20) << "Parallel (math.h)" 
                  << std::setw(15) << num_threads 
                  << std::setw(15) << (end_time - start_time) * 1000.0 << std::endl;
    }
    
    // Parallel implementations using Taylor series
    for (int num_threads = 2; num_threads <= max_threads; num_threads *= 2) {
        start_time = omp_get_wtime();
        compute_circle_points(center_x, center_y, radius, points, true, num_threads);
        end_time = omp_get_wtime();
        std::cout << std::left << std::setw(20) << "Parallel (Taylor)" 
                  << std::setw(15) << num_threads 
                  << std::setw(15) << (end_time - start_time) * 1000.0 << std::endl;
    }
    
    // Draw the circle (using the last computed points)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);
    draw_circle(renderer, points);
    SDL_RenderPresent(renderer);
    
    // Wait for user to close the window
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }
    
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}