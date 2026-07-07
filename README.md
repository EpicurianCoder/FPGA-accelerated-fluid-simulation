# Kria-LBM: FPGA Accelerated Fluid Simulation

## Project Overview

This project implements a high-performance computational fluid dynamics (CFD) simulation on the AMD Kria KV260 System-on-Module. It leverages the **Lattice Boltzmann Method (LBM)** and the **D2Q9 velocity set** to simulate fluid flow.

The core physics engine is written in C++ and synthesized into a custom hardware IP block using **AMD Vitis HLS**. The host application and visualization are handled by the Kria's ARM processor using Python and the PYNQ framework.

## Project Status (Phase 1: Architecture)

Before developing the hardware kernel, the mathematical and architectural boundaries of the project were established to ensure compatibility with the FPGA fabric:

- **Algorithm Pivot:** Traditional macroscopic fluid solvers (such as Eulerian Navier-Stokes or Gauss-Seidel relaxation) were discarded because they require global grid sweeps that stall parallel hardware. The Lattice Boltzmann Method was selected because the core operations (Stream and Collide) are highly localized, making the algorithm ideally suited for DSP slices and hardware pipelining.
- **Hardware Scale Limits:** To prevent immediate bottlenecks with DDR4 memory and complex AXI4-Stream interfaces, the initial prototype is constrained to a **256x64 grid**. This dimensioning ensures the entire environment fits safely within the Kria's internal BRAM/URAM capacity.
- **Separation of Concerns:** The FPGA is strictly reserved for executing the LBM physics. All peripheral tasks—such as generating NACA airfoil boundary coordinates, reading sensor telemetry, and rendering pixels—are delegated to Python scripts running on the ARM processor.
- **Project Structure:** A strict folder hierarchy was established to isolate hardware code from software code, preventing the Vitis HLS compiler from attempting to synthesize testbenches or UI logic.

## Directory Structure

- `/src` - Contains the synthesizable C++ hardware kernel and headers (`kria_lbm.cpp`, `kria_lbm.h`).
- `/tb` - Contains the C++ testbench used strictly for CPU-based math verification (`lbm_testbench.cpp`).
- `/host` - Contains the Python control scripts and visualization utilities.
- `/data` - Contains generated obstacle maps (e.g., airfoil coordinates) and benchmark outputs.

## Next Steps

1. Define the core data structures and D2Q9 constants in `kria_lbm.h`.
2. Draft the localized math for the macroscopic and equilibrium calculations in `kria_lbm.cpp`.
3. Build a simple CPU testbench to verify the physics before invoking the Vitis HLS compiler.
