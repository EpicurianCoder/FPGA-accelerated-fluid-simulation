#include "../src/kria_lbm.h"
#include <iostream>

int main()
{
    std::cout << "LBM Testbench started..." << std::endl;

    static lbm_type grid_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS] = {0};
    static lbm_type grid_new_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS] = {0};
    static bool obstacle_map[GRID_HEIGHT][GRID_WIDTH] = {false};

    static lbm_type omega = 1.0f; // Viscosity / Relaxaion Parameter (Currenlty on 1: "instant equilibrium")

    std::cout << "Calling the LBM core function..." << std::endl;
    kria_lbm_core(grid_f, grid_new_f, obstacle_map, omega);

    std::cout << "CPU testing Done... " << std::endl;
}