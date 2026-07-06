# Quatromino Board Optimizer

![C++](https://img.shields.io/badge/C++-20-00599C?logo=cplusplus&logoColor=white)
![OpenMP](https://img.shields.io/badge/OpenMP-Parallel-005C8B?logoColor=white)
![MPI](https://img.shields.io/badge/MPI-Distributed-13709B?logoColor=white)
![CMake](https://img.shields.io/badge/CMake-Build-064F8C?logo=cmake&logoColor=white)

A C++ **Branch and Bound** solver for a weighted board-covering optimization problem, implemented four ways — sequential, two OpenMP shared-memory variants, and a distributed MPI variant — so their behaviour on an irregular search space can be compared head to head.

Developed as the semestral project for **NI-PDP – Parallel and Distributed Programming** at the Faculty of Information Technology, Czech Technical University in Prague (2025/2026). The focus is less on the puzzle itself than on how a problem's characteristics — irregular workload, dynamic pruning — decide which parallelization paradigm actually pays off.

---

## The Problem

Given a rectangular board of size *a* × *b* (with `3 ≤ a, b ≤ 20` and `a·b ≥ 15`) where every cell holds a cost in `⟨1, 100⟩`, place **quatromino** pieces of type **T** and **Z** so as to minimize the total cost of the cells left uncovered. Each piece may be freely rotated and flipped, yielding **eight distinct shapes**.

A valid covering must satisfy:

1. **Inside the board** — pieces stay entirely within bounds.
2. **No overlap** — no two pieces share a cell.
3. **Maximality** — once finished, no further piece can be placed anywhere.
4. **Balance** — the counts of T and Z pieces are equal or differ by at most one.
5. **Minimality** — the sum of uncovered cell costs is as small as possible.

The program prints the minimum uncovered cost together with the board layout, where each placed piece is labelled `T<i>` / `Z<i>` (with `i` a unique placement id) and uncovered cells keep their original value.

---

## Approach

### Sequential algorithm

The core is a recursive **Branch and Bound** search that traverses the state space depth-first. It walks the board cell by cell from the top-left corner, and at each cell either places a piece or leaves the cell empty before recursing on the next one.

- **Best-first ordering** — candidate placements are tried highest-coverage-value first, so strong solutions surface early and tighten the bound sooner.
- **Pruning** — a branch is abandoned when its partial cost already meets or exceeds the best known solution, when the T/Z balance can no longer be satisfied, or when a provably optimal solution has already been found elsewhere.
- **Canonical piece representation** — each shape is a set of relative coordinates anchored at its top-left cell. A placement therefore attaches unambiguously to the current cell without disturbing already-processed cells, which is what makes the **maximality** constraint tractable: once a cell is declared uncovered, that decision is final. Whether a cell *can* be left empty then reduces to checking whether a piece could still be placed "backwards" over it.

Key `SQMSolver` methods: `solve`, `solveState` (one recursive step), `isDeadEnd`, `canBeLeftEmpty`, `collectPossibleMoves`, and `countRemainingFreeCells` (the last board row can never be covered, so its free cells are costed directly).

### Parallelization strategies

| Variant | Mechanism | Strengths | Limitations |
| --- | --- | --- | --- |
| **OpenMP – task parallelism** | A new `#pragma omp task firstprivate(state)` per candidate placement, spawned only down to a fixed depth (5) and sequential below that | Adapts dynamically to workload imbalance; parallel exploration tightens pruning fast | Task-creation overhead dominates at very high thread counts |
| **OpenMP – data parallelism** | Master generates the initial branches down to depth 5, then runs a `#pragma omp parallel for schedule(dynamic)` over them | Simple and predictable; dynamic scheduling gives some load balancing | The static branch set can't react to the tree's irregular shape |
| **MPI – master/slave** | Master packs initial states (`MPI_Pack`) and hands them to slaves through a dynamic work queue (`MPI_Iprobe`) | Scales across machines in principle | Communication overhead outweighs the compute at this problem size |

Shared-state handling in the task variant is deliberately lightweight: the best-solution update takes an `atomic` read fast-path before entering a `critical` section, and the "optimal found" flag is only ever set to `true` (never reset), so a stale read can at worst delay termination slightly — it can't produce a wrong answer.

---

## Repository Structure

```
.
├── CMakeLists.txt              # CMake build (OpenMP + MPI required)
├── Makefile                    # Cluster build (uses the CC compiler wrapper)
├── main.cpp                    # CLI entry point
├── data/                       # Sample board instances (mapaW_H.txt) + timing_tests/
├── include/
│   ├── Board.h, BoardState.h, Coords.h, Quatromin.h, ...
│   └── Solvers/
│       ├── SQMSolver.h              # shared base
│       ├── SQMSolverSeq.h
│       ├── SQMSolverTaskParallel.h
│       ├── SQMSolverDataParallel.h
│       └── SQMSolverMPI.h
├── src/
│   ├── Board.cpp, BoardReader.cpp, Quatromin.cpp, ...
│   └── Solvers/
│       ├── SQMSolver.cpp
│       ├── SQMSolverSeq.cpp
│       ├── SQMSolverTaskParallel.cpp
│       ├── SQMSolverDataParallel.cpp
│       └── SQMSolverMPI.cpp
└── tests/
    ├── SolverTester.cpp        # solver correctness tests
    ├── CLITester.cpp           # CLI argument-parsing tests
    └── SolverBenchmark.py      # benchmarking helper
```

---

## Building and Running

**Prerequisites**

- A C++20 compiler
- CMake 3.1+
- OpenMP and MPI

---

**Compiling**

```bash
make            # produces build/app
make clean      # remove build artifacts
```

---

**Running**

```bash
# Sequential (baseline)
./Solver --board ../data/mapa5_11.txt --solver sequential

# OpenMP task parallelism
./Solver --board ../data/mapa5_11.txt --solver task-parallel --threads 8

# OpenMP data parallelism
./Solver --board ../data/mapa5_11.txt --solver data-parallel --threads 8

# MPI (process count comes from the launcher)
mpirun -np 4 ./Solver --board ../data/mapa5_11.txt --solver mpi
```

Run `./Solver --help` for the full option list (`--board`, `--solver`, `--threads`, `--nodes`).

**Board file format** — first line is `width height`, followed by the grid of cell costs:

```
5 11
    9    25    33    59     9    10     7    22    68     3    11
    6    42    15     9     9     8    25    38    57    65    26
   51    25    23    82    83    15    90    11    15    32    19
   50    14    84    41    16    26    18    70    17    90    25
   89    84    33    10    89    86    25    67    21    66    21
```

**Example output**

```
Board loaded successfully (size: 11x5)
Starting solve with Sequential solver

Solution found with value: 18
Board configuration:
T1 T1 T1  9 ...
 6 T1 15  9 ...
Z1 Z1 Z2 Z2 ...
...
```

---

## Testing

The CMake build produces test executables (registered with CTest):

```bash
cd build
ctest --output-on-failure
```

---

## Results

Measured on the FIT cluster (ClusterFIT). Speedups in parentheses are for the best thread/process count observed; speedup is `S = T(1) / T(p)`.

| Instance | Size | Sequential |    OpenMP Task |    OpenMP Data | MPI |
| --- | --- | ---: |---------------:|---------------:| ---: |
| `mapa5_11.txt` | small | 2.2 s |   0.3 s (7.3×) |   0.4 s (5.5×) | 47.7 s* |
| `mapa9_9.txt` | medium | 90 s |  7.8 s (11.5×) |  23.5 s (3.8×) | timeout |
| `mapa5_15.txt` | large | 126 s |  7.2 s (17.5×) |  29.9 s (4.2×) | timeout |

<sub>*MPI was **slower than the sequential baseline** even on the small instance and timed out on the others — communication and state-packing overhead dominates at this problem scale.</sub>

**What the numbers show**

- **Task parallelism wins.** It fits Branch and Bound naturally — work is handed out as the tree is discovered, so uneven branch sizes balance themselves. Larger instances even show **superlinear speedup** (≈36×): parallel exploration finds a good solution sooner, which sharpens pruning and drives the total number of states explored below the sequential run.
- **Data parallelism is stable but modest** (≈6–7×). The set of branches is fixed up front, so it can't react to the wildly different cost of each subtree; some threads finish early while others grind on.
- **MPI underperforms.** Packing and shipping states between processes, plus synchronizing the global best, costs more than it saves at this problem scale. Larger instances didn't finish within the cluster time limit.
- **Scaling isn't linear.** All variants hit diminishing returns past ~16–32 threads as scheduling, synchronization, and best-solution contention grow.

---

## Technology Stack

- **Language:** C++20
- **Shared memory:** OpenMP (tasks and `parallel for`)
- **Distributed:** MPI (master/slave, `MPI_Pack` / `MPI_Iprobe`)
- **Build:** CMake / Makefile
- **Tests :** C++ test drivers 
