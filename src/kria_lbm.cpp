#include "kria_lbm.h"

// HELPER FUNCTION: COLLISION
// Calculates macroscopic density/velocity and relaxes towards equilibrium.
// NB: f is the particle distribution function!
// ============================================================================
void compute_collision(lbm_type f[NUM_DIRECTIONS], lbm_type new_f[NUM_DIRECTIONS], lbm_type omega)
{
    // #pragma HLS INLINE
    // HLS INLINE PRAGMA will be added here at a later stage
    // Instructs the compiler to dissolve this function into the parent loop.

    // rho is mass per unit volume in physics
    lbm_type rho = 0.0f;
    lbm_type ux = 0.0f;
    lbm_type uy = 0.0f;

    // calculates the sum of velocity of the particles and vectors
    for (int i = 0; i < NUM_DIRECTIONS; i++)
    {
        // for each vector, the
        rho += f[i];
        ux += f[i] * dirX[i];
        uy += f[i] * dirY[i];
    }

    ux /= rho;
    uy /= rho;

    // START: Calculate equilibrium and apply BGK collision
    // ---------------------------------------------
    // calculates the velocity squared
    lbm_type u_sq = ux * ux + uy * uy;
    lbm_type temp_1 = u_sq * 1.5f;

    for (int i = 0; i < NUM_DIRECTIONS; i++)
    {
        lbm_type cu = dirX[i] * ux + dirY[i] * uy;
        lbm_type temp_2 = cu * 3.0f;

        // Equilibrium Function
        lbm_type feq = weights[i] * rho * (1.0f + temp_2 + 0.5f * temp_2 * temp_2 - temp_1);

        // BGK Collisions
        new_f[i] = (1.0f - omega) * f[i] + omega * feq;
    }
    // END -----------------------------------------
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

            // HLS PIPELINE PRAGMA will go here
            // #pragma HLS PIPELINE II=1
            // TO DO: Tell the FPGA to process one pixel per clock cycle

            if (obstacle_map[y][x])
            {
                // Fluid hitting an obstacle cell reflects 180 degrees back
                // into its source direction, conserving momentum and mass.
                for (int i = 0; i < NUM_DIRECTIONS; i++)
                {
                    // direction[] is a look up table that reverses direction N -> S
                    int inv_dir = opposite[i];

                    // Apply these new vectors to the grid
                    grid_new_f[y][x][inv_dir] = grid_f[y][x][i];
                }
            }
            else
            {
                // fetch the9 values from the current pixel [y][x]
                lbm_type local_f[NUM_DIRECTIONS];
                lbm_type new_local_f[NUM_DIRECTIONS];

                for (int i = 0; i < NUM_DIRECTIONS; i++)
                {
                    local_f[i] = grid_f[y][x][i];
                }

                compute_collision(local_f, new_local_f, omega);

                // Writes the updated values back to the "next" state grid
                for (int i = 0; i < NUM_DIRECTIONS; i++)
                {
                    grid_new_f[y][x][i] = new_local_f[i];
                }
            }
        }
    }
    // END : Phase 1 -------------------------------

    // STREAMING LOOP
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {

            for (int i = 0; i < NUM_DIRECTIONS; i++)
            {

                // Calculate the coordinates for the neighbours of the target cell
                int next_y = y + dirY[i];
                int next_x = x + dirX[i];

                // Only stream if the destination is safely inside the grid boundaries
                if (next_x >= 0 && next_x < GRID_WIDTH && next_y >= 0 && next_y < GRID_HEIGHT)
                {
                    grid_f[next_y][next_x][i] = grid_new_f[y][x][i];
                }
            }
        }
    }
    // END : Phase 2 -------------------------------
}