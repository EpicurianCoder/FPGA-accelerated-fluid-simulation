#include "kria_lbm.h"

// HELPER FUNCTION: COLLISION
// Calculates macroscopic density/velocity and relaxes towards equilibrium.
void compute_collision(lbm_type f[NUM_DIRECTIONS], lbm_type new_f[NUM_DIRECTIONS], lbm_type omega)
{
    // #pragma HLS INLINE

    // TO DO:
    // Forces HLS to unroll this function directly into the parent loop

    // PLACEHOLDER FOR COLLISON CALCS:
    // Calculate local density (rho) and velocity (ux, uy).
    // Calculate the equilibrium distribution (feq).
    // Apply the BGK collision formula to write to `new_f`.
}

// TOP-LEVEL HARDWARE KERNEL
// Vitis HLS will synthesize this into the custom IP block eventually.
void kria_lbm_core(lbm_type grid_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS],
                   lbm_type grid_new_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS],
                   bool obstacle_map[GRID_HEIGHT][GRID_WIDTH],
                   lbm_type omega)
{

    // HLS INTERFACE PRAGMAS
    // #pragma HLS INTERFACE --> m_axi (x3) + s_axilite (x2)

    // HLS MEMORY PRAGMAS
    // Split the 9 directions into separate memory 'banks'
    // This lets it read all 9 in 1 clock cycle
    // #pragma HLS ARRAY_PARTITION --> grif_f + grid_f_new

    // COLLISION LOOP
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            // #pragma HLS PIPELINE II=1
            // TO DO: Tell the FPGA to process one pixel per clock cycle

            if (obstacle_map[y][x])
            {
                // TO DO: Bounce-back boundary condition for walls
            }
            else
            {
                // Fetch the 9 values for the current pixel
                lbm_type local_f[NUM_DIRECTIONS];
                lbm_type local_new_f[NUM_DIRECTIONS];

                for (int i = 0; i < NUM_DIRECTIONS; i++)
                {
                    local_f[i] = grid_f[y][x][i];
                }

                // Compute the physics
                compute_collision(local_f, local_new_f, omega);

                // Write the updated values back to the "next" state grid
                for (int i = 0; i < NUM_DIRECTIONS; i++)
                {
                    grid_new_f[y][x][i] = local_new_f[i];
                }
            }
        }
    }

    // STREAMING LOOP
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            // #pragma HLS PIPELINE II=1

            // PLACEHOLDER:
            // Read from grid_new_f[y][x] and write to grid_f[y+dirY][x+dirX].
            // Note: Boundary conditions (wrapping around edges) will need to be handled here.
        }
    }
}