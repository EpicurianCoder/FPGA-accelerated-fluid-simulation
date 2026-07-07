# Kria-LBM: FPGA Accelerated Fluid Simulation

## Project Overview

This project implements a high-performance computational fluid dynamics (CFD) simulation on the AMD Kria KV260 System-on-Module. It leverages the **Lattice Boltzmann Method (LBM)** and the **D2Q9 velocity set** to simulate fluid flow.

The core physics engine is written in C++ and synthesized into a custom hardware IP block using **AMD Vitis HLS**. The host application and visualization are handled by the Kria's ARM processor using Python and the PYNQ framework.

## Project Status

### Phase 1: Architecture (Complete)

- **Algorithm Pivot:** Traditional macroscopic fluid solvers (such as Eulerian Navier-Stokes or Gauss-Seidel relaxation) were discarded. The Lattice Boltzmann Method was selected because the core operations (Stream and Collide) are highly localized, making the algorithm ideally suited for DSP slices and hardware pipelining.
- **Hardware Scale Limits:** The initial prototype is constrained to a **256x64 grid** to ensure the entire environment fits safely within the Kria's internal BRAM/URAM capacity, bypassing DDR4 memory bottlenecks.
- **Separation of Concerns:** The FPGA is strictly reserved for executing the LBM physics. Peripheral tasks (sensor telemetry, UI rendering) are delegated to Python scripts on the ARM processor.

### Phase 2: Software Baseline (In Progress)

- **Data Structures:** The D2Q9 physics constants, velocity vectors, and grid limits have been defined in the core header file.
- **Physics Engine:** The localized BGK collision mathematics (calculating macroscopic density/velocity, equilibrium distribution, and relaxation) have been successfully implemented.
- **CPU Testbench:** A basic C++ testing environment has been established to allocate heap memory and verify mathematical execution on the CPU prior to hardware synthesis.

## Directory Structure

- `/src` - Contains the synthesizable C++ hardware kernel and headers (`kria_lbm.cpp`, `kria_lbm.h`).
- `/tb` - Contains the C++ testbench used strictly for CPU-based math verification (`lbm_testbench.cpp`).
- `/host` - Contains the Python control scripts and visualization utilities.
- `/data` - Contains generated obstacle maps (e.g., airfoil coordinates) and benchmark outputs.

## Next Steps

1. Define the core data structures and D2Q9 constants in `kria_lbm.h`.
2. Draft the localized math for the macroscopic and equilibrium calculations in `kria_lbm.cpp`.
3. Build a simple CPU testbench to verify the physics before invoking the Vitis HLS compiler.
