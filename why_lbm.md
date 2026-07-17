## Why use the Lattice Boltzmann Method? And what are the alternatives?

### Navier-Stokes and Poisson Solver

Originally, I was going to use the Stable Fluids solution by Jos Stam (Stable Fluids), which was a development on earlier particle simulation methods that involved the Navier-Stokes equation, but focused specifically on creating stability for use within the gaming industry.

**Why not use this?**
After doing some research, both the older, ‘less stable’ Navier-Stokes methods as well as the newer “Stable Fluids” method developed by Jos Stam, take advantage of a Poisson equation to solve the pressure across the different cells in the particle simulation. This method for solving the pressure operates globally and involves the need to perform mathematical operations on more than simply the current particle. The projection also uses a Poisson solver, another global method.

---

## GPU vs FPGA

### GPU

One can think of a GPU as a set of incredibly fast dedicated hardware units that perform operations in parallel. The number of instructions they can receive, and actions they can perform, are finite and these instructions are sent to these parallel compute engines in incredibly fast cycles.

In each cycle, the GPU performs operations as per the current instruction and inputs, after which it returns control to a global function. This in turn once again hands a new set of instructions to the parallel machines. This allows for incredibly powerful and effective parallelism.

It is however limited. To perform a full operational pipeline, perhaps consisting of multiple operations and inputs, the parallel engines within the GPU are required to constantly be told which operations to perform, and which inputs to use, and then are required to perform this repeatedly for every operation.

While this happens incredibly fast, it requires a break in this high-speed cycle to communicate with other hardware, access global memory objects, and will have bottleneck scenarios in scheduling and memory access.

### FPGA

Field Programmable Gate Arrays (FPGAs) come as a blank slate of entirely definable logic gates that we have full control over. This means we can determine how many individual processes we would like to support in parallel, and we can decide the combination of operations we need for our “entire pipeline”. In the same way that our GPU is able to use its combination of available high-speed instructions to perform our pipeline of actions over a sequence of iterations, our FPGA can instead be handed all these operations as a chain and can configure the internal logic gates to perform that specific sequence of actions, without having to repeatedly return to a global controller to receive the next instruction in the chain.

### Implications

As a result, the FPGA can perform back-to-back mathematical operations and end-to-end pipeline equations incredibly fast. For this reason, we can define early on exactly the memory addresses and places from where the inputs for each action will come and additionally prevent the overhead need to perform global memory access. What this does mean, is that any operations performed using global variables, in which particles require knowledge of each other’s state at the same moment, are not ideal for use in a system with such a design.
