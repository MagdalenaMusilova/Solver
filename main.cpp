
#include <iostream>
#include <string>
#include <memory>
#include <cstring>

#include "include/BoardReader.h"
#include "include/Board.h"
#include "include/SQMSolution.h"
#include "include/Solvers/SQMSolverSeq.h"
#include "include/Solvers/SQMSolverTaskParallel.h"
#include "include/Solvers/SQMSolverDataParallel.h"
#include "include/Solvers/SQMSolverMPI.h"

enum class SolverType {
    SEQUENTIAL,
    TASK_PARALLEL,
    DATA_PARALLEL,
    MPI
};

struct CLIOptions {
    std::string boardFile;
    SolverType solverType = SolverType::SEQUENTIAL;
    int numThreads = 1;
    int numNodes = 1;
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " --board <file> [OPTIONS]\n\n";
    std::cout << "Required arguments:\n";
    std::cout << "  --board <file>              Path to the board file\n\n";
    std::cout << "Optional arguments:\n";
    std::cout << "  --solver <type>             Solver type (default: sequential)\n";
    std::cout << "                              Options: sequential, task-parallel, data-parallel, mpi\n";
    std::cout << "  --threads <num>             Number of threads to use (default: 1)\n";
    std::cout << "  --nodes <num>               Number of MPI nodes to use (only with mpi solver)\n";
    std::cout << "  --help                      Show this help message\n";
}

bool parseCLI(int argc, char* argv[], CLIOptions& options) {
    if (argc < 2) {
        std::cerr << "Error: Missing required arguments\n";
        printUsage(argv[0]);
        return false;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--board") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --board requires a file path argument\n";
                return false;
            }
            options.boardFile = argv[++i];
        } else if (arg == "--solver") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --solver requires a solver type argument\n";
                return false;
            }
            std::string solverStr = argv[++i];
            if (solverStr == "sequential") {
                options.solverType = SolverType::SEQUENTIAL;
            } else if (solverStr == "task-parallel") {
                options.solverType = SolverType::TASK_PARALLEL;
            } else if (solverStr == "data-parallel") {
                options.solverType = SolverType::DATA_PARALLEL;
            } else if (solverStr == "mpi") {
                options.solverType = SolverType::MPI;
            } else {
                std::cerr << "Error: Unknown solver type '" << solverStr << "'\n";
                return false;
            }
        } else if (arg == "--threads") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --threads requires a number argument\n";
                return false;
            }
            options.numThreads = std::stoi(argv[++i]);
            if (options.numThreads < 1) {
                std::cerr << "Error: Number of threads must be at least 1\n";
                return false;
            }
        } else if (arg == "--nodes") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --nodes requires a number argument\n";
                return false;
            }
            options.numNodes = std::stoi(argv[++i]);
            if (options.numNodes < 1) {
                std::cerr << "Error: Number of nodes must be at least 1\n";
                return false;
            }
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            printUsage(argv[0]);
            return false;
        }
    }

    if (options.boardFile.empty()) {
        std::cerr << "Error: --board argument is required\n";
        return false;
    }

    if (options.solverType == SolverType::MPI && options.numNodes > 1) {
        std::cout << "Info: Using MPI solver with " << options.numNodes << " nodes\n";
    }

    return true;
}

std::unique_ptr<SQMSolver> createSolver(const Board& board, const CLIOptions& options) {
    switch (options.solverType) {
        case SolverType::SEQUENTIAL:
            return std::make_unique<SQMSolverSeq>(board);
        case SolverType::TASK_PARALLEL:
            return std::make_unique<SQMSolverTaskParallel>(board);
        case SolverType::DATA_PARALLEL:
            return std::make_unique<SQMSolverDataParallel>(board);
        case SolverType::MPI:
            return std::make_unique<SQMSolverMPI>(board);
        default:
            return std::make_unique<SQMSolverSeq>(board);
    }
}

int main(int argc, char* argv[]) {
    CLIOptions options;

    if (!parseCLI(argc, argv, options)) {
        return 1;
    }

    // Load the board from file
    Board board;
    if (!BoardReader::ReadBoard(options.boardFile, board)) {
        std::cerr << "Error: Failed to read board from file '" << options.boardFile << "'\n";
        return 1;
    }

    std::cout << "Board loaded successfully (size: " << board.width << "x" << board.height << ")\n";

    // Create appropriate solver
    auto solver = createSolver(board, options);

    std::cout << "Starting solve with " << argv[0];
    switch (options.solverType) {
        case SolverType::SEQUENTIAL:
            std::cout << "Sequential solver";
            break;
        case SolverType::TASK_PARALLEL:
            std::cout << "Task parallel with " << options.numThreads << " threads";
            break;
        case SolverType::DATA_PARALLEL:
            std::cout << "Data parallel with " << options.numThreads << " threads";
            break;
        case SolverType::MPI:
            std::cout << "MPI with " << options.numNodes << " nodes and " << options.numThreads << " threads";
            break;
    }
    std::cout << "\n";

    // Solve the puzzle
    SQMSolution solution = solver->solve();

    // Display results
    std::cout << "\nSolution found with value: " << solution.value << "\n";
    std::cout << "Board configuration:\n";
    for (const auto& row : solution.filledInBoard) {
        for (const auto& cell : row) {
            std::cout << cell;
        }
        std::cout << "\n";
    }

    return 0;
}