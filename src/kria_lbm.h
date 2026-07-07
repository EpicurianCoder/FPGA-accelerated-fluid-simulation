#ifndef KRIA_LBM_H
#define KRIA_LBM_H

// DEFINES & CONFIG
// Small (256*64) so the entire grid fits on the FPGA's internal BRAM/URAM.
#define GRID_WIDTH 256
#define GRID_HEIGHT 64
#define NUM_DIRECTIONS 9

// DATA TYPES
// Start with standard floats for easy CPU validation.
// Later, I will swap this to `half` or `ap_fixed` to save FPGA fabric space.
typedef float lbm_type;

// D2Q9 CONSTANTS
// These are static constant arrays.
// Eventualy I will use Vitis HLS to hardwire these directly into logic gates
const int dirX[NUM_DIRECTIONS] = {0, 1, 0, -1, 0, 1, -1, -1, 1};
const int dirY[NUM_DIRECTIONS] = {0, 0, -1, 0, 1, -1, -1, 1, 1};

const lbm_type weights[NUM_DIRECTIONS] = {
    4.0f / 9.0f,
    1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f,
    1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f};

// Indices for bounce-back boundary conditions (reversing direction)
const int opposite[NUM_DIRECTIONS] = {0, 3, 4, 1, 2, 7, 8, 5, 6};

// FUNCTION PROTOTYPES
void kria_lbm_core(lbm_type grid_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS],
                   lbm_type grid_new_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS],
                   bool obstacle_map[GRID_HEIGHT][GRID_WIDTH],
                   lbm_type omega);

#endif // KRIA_LBM_H