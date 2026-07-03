#include <iostream>
#include <cassert>
#include <cmath>
#include <chrono>
#include <memory>
#include <iomanip>

#include "../include/BoardReader.h"
#include "../include/Board.h"
#include "../include/SQMSolution.h"
#include "../include/Solvers/SQMSolverSeq.h"
#include "../include/Solvers/SQMSolverTaskParallel.h"
#include "../include/Solvers/SQMSolverDataParallel.h"

// Test data with correct solutions
struct TestCase {
    std::string filePath;
    int expectedValue;
};

class SolverTester {
private:
    std::vector<TestCase> testCases = {
        {"data/mapa3_5.txt", 25},
        {"data/mapa5_5.txt", 23},
        {"data/mapa3_11.txt", 33},
        {"data/mapa7_7.txt", 10},
        {"data/mapa4_15.txt", 0},
        {"data/mapa5_11.txt", 18},
        {"data/mapa7_10.txt", 5},
        {"data/mapa5_15.txt", 8},
        {"data/mapa7_11.txt", 5},
        {"data/mapa15_5.txt", 5},
        {"data/mapa9_9.txt", 5}
    };

    int passedTests = 0;
    int failedTests = 0;
    double totalTime = 0.0;

public:
    void runAllTests() {
        std::cout << "===============================================\n";
        std::cout << "         Running Solver Integration Tests        \n";
        std::cout << "===============================================\n\n";

        for (const auto& testCase : testCases) {
            testSolver(testCase);
        }

        std::cout << "\n===============================================\n";
        std::cout << "                    Test Results                \n";
        std::cout << "===============================================\n";
        std::cout << "Passed: " << passedTests << "\n";
        std::cout << "Failed: " << failedTests << "\n";
        std::cout << "Total:  " << (passedTests + failedTests) << "\n";
        std::cout << "Total Time: " << std::fixed << std::setprecision(3) 
                  << totalTime << "s\n";
        std::cout << "===============================================\n";

        if (failedTests > 0) {
            exit(1);
        }
    }

private:
    void testSolver(const TestCase& testCase) {
        std::cout << "Testing: " << testCase.filePath << " ... ";
        std::cout.flush();

        // Load board
        Board board;
        if (!BoardReader::ReadBoard(testCase.filePath, board)) {
            std::cout << "FAILED (Could not read board file)\n";
            failedTests++;
            return;
        }

        // Measure time
        auto start = std::chrono::high_resolution_clock::now();

        // Solve with sequential solver
        SQMSolverSeq solver(board);
        SQMSolution solution = solver.solve();

        auto end = std::chrono::high_resolution_clock::now();
        double elapsedSeconds = std::chrono::duration<double>(end - start).count();
        totalTime += elapsedSeconds;

        // Check result
        if (solution.value == testCase.expectedValue) {
            std::cout << "PASSED";
            passedTests++;
        } else {
            std::cout << "FAILED";
            failedTests++;
            std::cout << " (Expected: " << testCase.expectedValue 
                      << ", Got: " << solution.value << ")";
        }

        std::cout << " [" << std::fixed << std::setprecision(3) 
                  << elapsedSeconds << "s]\n";
    }
};

int main() {
    SolverTester tester;
    tester.runAllTests();
    return 0;
}