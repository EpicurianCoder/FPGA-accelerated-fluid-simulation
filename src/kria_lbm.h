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
// eg (dirX[2], dirY[2]) would be the third "direction" in those combined arrays
// with the value (0, -1) representing North ^
const int dirX[NUM_DIRECTIONS] = {0, 1, 0, -1, 0, 1, -1, -1, 1};
const int dirY[NUM_DIRECTIONS] = {0, 0, -1, 0, 1, -1, -1, 1, 1};

// Indices for bounce-back boundary conditions (reversing direction)
// eg. Taking (dirX[2], dirY[2]) and applying opposite[2] would be (dirX[2], opposite[2])
// (0, -1) NORTH becomes (0, 1) SOUTH
const int opposite[NUM_DIRECTIONS] = {0, 3, 4, 1, 2, 7, 8, 5, 6};

// IMPORTANT LAYOUT FOR 9-CELL GRID !!!

// Bucket 0 (Center)                (0, 0)
// Bucket 1 (East, +X)              (1, 0)
// Bucket 2 (North, -Y)             (0, -1)
// Bucket 3 (West, -X)              (-1, 0)
// Bucket 4 (South, +Y)             (0, 1)
// Bucket 5 (North-East, +X, -Y)    (1, -1)
// Bucket 6 (North-West, -X, -Y)    (-1, -1)
// Bucket 7 (South-West, -X, +Y)    (-1, 1)
// Bucket 8 (South-East, +X, +Y)    (1, 1)

// These values are the basis for The Maxwell-Boltzmann Distribution
// This allows us to treat the diaognal distances in same manner as the horiz/vert
// It is the probable distribution of mass, reflected as ratios
const lbm_type weights[NUM_DIRECTIONS] = {
    4.0f / 9.0f,
    1.0f / 9.0f,
    1.0f / 9.0f,
    1.0f / 9.0f,
    1.0f / 9.0f,
    1.0f / 36.0f,
    1.0f / 36.0f,
    1.0f / 36.0f,
    1.0f / 36.0f};

// FUNCTION PROTOTYPES
void kria_lbm_core(lbm_type grid_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS],
                   lbm_type grid_new_f[GRID_HEIGHT][GRID_WIDTH][NUM_DIRECTIONS],
                   bool obstacle_map[GRID_HEIGHT][GRID_WIDTH],
                   lbm_type omega);

#endif // KRIA_LBM_H