#include "../src/kria_lbm.h"
#include <iostream>
// Set of 'manipulator' functions allow precise control over how numbers look on the screen
#include <iomanip>

int main()
{
    std::cout << "LBM Testbench started..." << std::endl;

    // allocates memory on the heap
    static lbm_type grid_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS] = {0};
    static lbm_type grid_new_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS] = {0};
    static bool obstacle_map[GRID_HEIGHT][GRID_WIDTH] = {false};

    // Sim params
    static lbm_type omega = 1.5f; // Viscosity / Relaxaion Parameter (Using a real value: 1.,5f)
    int total_steps = 100;

    // START: Phase 1 - Initialize the fluid State
    // This is a uniform 'wind' from left to right (ux = 0.1, uy = 0.0)
    // ---------------------------------------------
    lbm_type init_rho = 1.0f;
    lbm_type init_ux = 0.1f;
    lbm_type init_uy = 0.0f;
    lbm_type init_u_sq = init_ux * init_ux + init_uy * init_uy;

    // Define a vertical obstacle in the flow channel
    int obstacle_x = 32;
    int obstacle_y_start = (GRID_HEIGHT / 2) - 8; // Row 24
    int obstacle_y_end = (GRID_HEIGHT / 2) + 8;   // Row 40

    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            // Place the solid barrier map coordinates
            if (x == obstacle_x && y >= obstacle_y_start && y <= obstacle_y_end)
            {
                obstacle_map[y][x] = true;
            }
            else
            {
                obstacle_map[y][x] = false;
            }

            for (int i = 0; i < NUM_DIRECTIONS; i++)
            {
                lbm_type cu = dirX[i] * init_ux + dirY[i] * init_uy;

                // Apply the equilibirium function
                grid_f[y][x][i] = weights[i] * init_rho * (1.0f + 3.0f * cu + 4.5f * cu * cu - 1.5f * init_u_sq);
            }
        }
    }
    // END : Phase 1 -------------------------------

    std::cout << "Fluid initialized. Running " << total_steps << " iterations..." << std::endl;

    // START: Phase 2 - Run Simulation for number of chosen steps in 'total_steps'
    // ---------------------------------------------
    for (int step = 0; step < total_steps; step++)
    {
        kria_lbm_core(grid_f, grid_new_f, obstacle_map, omega);
    }
    std::cout << "Fluid Simulation Ended." << std::endl;
    // END : Phase 2 -------------------------------

    // START: Phase 3 - Result Checking
    // ---------------------------------------------
    int check_x = GRID_WIDTH / 2; // Column 128 (Downstream wake)
    int check_y = GRID_HEIGHT / 2;

    lbm_type final_rho = 0.0f;
    lbm_type final_ux = 0.0f;

    // checks results against expected results
    for (int i = 0; i < NUM_DIRECTIONS; i++)
    {
        final_rho += grid_f[check_y][check_x][i];
        final_ux += grid_f[check_y][check_x][i] * dirX[i];
    }
    final_ux /= final_rho;

    std::cout << "--- Test Complete ---" << std::endl;
    // std::fixed > print floating-point nicely (1e-05 displayed as 0.00001)
    // std::setprecision(5) > If the density 1,forces print as 1.00000
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Target Density:  1.00000  | Final Density:  " << final_rho << std::endl;
    std::cout << "Target Velocity: 0.10000  | Final Velocity: " << final_ux << std::endl;

    // Basic pass/fail check test requirements
    if (final_rho > 0.99f && final_rho < 1.01f)
    {
        std::cout << "RESULT: PASS (Mass conserved)" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "RESULT: FAIL (Mass explosion detected)" << std::endl;
        return 1;
    }

    std::cout << "CPU testing Done... " << std::endl;
}