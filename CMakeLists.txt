cmake_minimum_required(VERSION 3.1)
project(Solver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

# Main executable
add_executable(Solver
        main.cpp

        src/Board.cpp
        src/BoardFilling.cpp
        src/BoardReader.cpp
        src/BoardState.cpp
        src/Coords.cpp
        src/PosedQuatromin.cpp
        src/Quatromin.cpp
        src/SQMSolution.cpp

        src/Solvers/SQMSolver.cpp
        src/Solvers/SQMSolverSeq.cpp
        src/Solvers/SQMSolverDataParallel.cpp
        src/Solvers/SQMSolverTaskParallel.cpp
        src/Solvers/SQMSolverMPI.cpp

        include/Solvers/SQMSolverSeq.h
        include/Solvers/SQMSolverDataParallel.h
        include/Solvers/SQMSolverTaskParallel.h
        include/Solvers/SQMSolverMPI.h
)

target_link_libraries(Solver PRIVATE
        OpenMP::OpenMP_CXX
        MPI::MPI_CXX
)

# Test executables
enable_testing()

# Integration tests
add_executable(test_solver tests/test_solver.cpp
        src/Board.cpp
        src/BoardFilling.cpp
        src/BoardReader.cpp
        src/BoardState.cpp
        src/Coords.cpp
        src/PosedQuatromin.cpp
        src/Quatromin.cpp
        src/SQMSolution.cpp

        src/Solvers/SQMSolver.cpp
        src/Solvers/SQMSolverSeq.cpp
        src/Solvers/SQMSolverDataParallel.cpp
        src/Solvers/SQMSolverTaskParallel.cpp
        src/Solvers/SQMSolverMPI.cpp
)

target_link_libraries(test_solver PRIVATE
        OpenMP::OpenMP_CXX
        MPI::MPI_CXX
)

# CLI tests
add_executable(test_cli tests/test_cli.cpp)

add_test(NAME SolverIntegrationTests COMMAND test_solver)
add_test(NAME CLITests COMMAND test_cli)