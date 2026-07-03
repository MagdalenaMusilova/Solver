//
// Created by majda on 5/25/2026.
//

#include "../../include/Solvers/SQMSolverMPI.h"

#include <mpi.h>
#include <queue>

bool SQMSolverMPI::tryUpdateBestValue(int newValue) {
    bool res = false;

    int currentBest;
#pragma omp atomic read
    currentBest = bestValue;

    if (newValue < currentBest) {
#pragma omp critical
        {
            if (newValue < bestValue) {
                bestValue = newValue;
                if (newValue == trivialBound) {
                    isSolved = true;
                }
                res = true;
            }
        }
    }
    return res;
}

void SQMSolverMPI::sendBoardState(BoardState state, int recievingRank, int tag) const {
    char buffer[BUFFER_SIZE];
    int position=0;
    // send the easy stuff
    MPI_Pack(&bestValue, 1, MPI_INT, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);
    MPI_Pack(&state.pos.x, 1, MPI_CHAR, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);
    MPI_Pack(&state.pos.y, 1, MPI_CHAR, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);
    MPI_Pack(&state.value, 1, MPI_INT, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);
    MPI_Pack(&state.numOfZ, 1, MPI_UNSIGNED_CHAR, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);
    MPI_Pack(&state.numOfT, 1, MPI_UNSIGNED_CHAR, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);
    // send how the board is filled
    std::uint8_t boardBuffer[MAX_BOARD_SIZE * sizeof(BoardFilling)];
    auto boardBufferSize = state.feedBoardIntoBuffer(boardBuffer);
    MPI_Pack(&boardBufferSize, 1, MPI_INT, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);
    MPI_Pack(boardBuffer, boardBufferSize, MPI_UNSIGNED_CHAR, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);

    MPI_Send (buffer, position, MPI_PACKED, recievingRank, tag, MPI_COMM_WORLD);
}

BoardState SQMSolverMPI::recieveBoardState(int tag, int& recievedBestValue) {
    char buffer[BUFFER_SIZE];
    int position=0;
    MPI_Recv(buffer, BUFFER_SIZE, MPI_PACKED, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // unpack easy stuff
    MPI_Unpack(buffer, BUFFER_SIZE, &position, &recievedBestValue, 1, MPI_INT, MPI_COMM_WORLD);
    Coords nextPos;
    MPI_Unpack(buffer, BUFFER_SIZE, &position, &nextPos.x, 1, MPI_CHAR, MPI_COMM_WORLD);
    MPI_Unpack(buffer, BUFFER_SIZE, &position, &nextPos.y, 1, MPI_CHAR, MPI_COMM_WORLD);
    int value;
    MPI_Unpack(buffer, BUFFER_SIZE, &position, &value, 1, MPI_INT, MPI_COMM_WORLD);
    std::uint8_t numOfZ, numOfT;
    MPI_Unpack(buffer, BUFFER_SIZE, &position, &numOfZ, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
    MPI_Unpack(buffer, BUFFER_SIZE, &position, &numOfT, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
    // unpack board
    std::uint8_t boardBuffer[MAX_BOARD_SIZE * sizeof(BoardFilling)];
    int boardBufferSize;
    MPI_Unpack(buffer, BUFFER_SIZE, &position, &boardBufferSize, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, BUFFER_SIZE, &position, boardBuffer, boardBufferSize, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
    return {board.width, nextPos, value, numOfZ, numOfT, boardBuffer, boardBufferSize};
}

void SQMSolverMPI::masterSolve(int numOfSlaves) {
    int bestRank = 0;

    std::queue<int> freeSlaves;
    for (int i = 1; i <= numOfSlaves; ++i) {
        freeSlaves.push(i);
    }

    std::vector<BoardState> tasksToSolve;
    BoardState firstState(board.width);
    solveStateAndReturn(firstState, NUM_OF_FIRST_MOVES, tasksToSolve);

    while (!tasksToSolve.empty() || freeSlaves.size() != static_cast<size_t>(numOfSlaves)) {

        // assign tasks to slaves
        while (!freeSlaves.empty() && !tasksToSolve.empty()) {
            BoardState task = tasksToSolve.back();
            tasksToSolve.pop_back();

            int slaveRank = freeSlaves.front();
            freeSlaves.pop();

            sendBoardState(task, slaveRank, TAG_TASK);
        }

        // receive results from slaves
        while (true) {
            int flag;
            MPI_Iprobe(MPI_ANY_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
            if (!flag) break;

            MPI_Status status;
            int foundBestVal;

            MPI_Recv(&foundBestVal, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &status);

            if (tryUpdateBestValue(foundBestVal)) {
                bestRank = status.MPI_SOURCE;
            }

            freeSlaves.push(status.MPI_SOURCE);
        }
    }

    // terminate slaves
    for (int slaveRank = 1; slaveRank <= numOfSlaves; ++slaveRank) {
        MPI_Send(&bestRank, 1, MPI_INT, slaveRank, TAG_TERMINATE, MPI_COMM_WORLD);
    }

    int dummy;
    if (bestRank != 0) {
        bestState = recieveBoardState(TAG_FINAL_RESULT, dummy);
    }
}

void SQMSolverMPI::slaveSolve(int rank) {
    bool terminated = false;
    while (!terminated) {
        MPI_Status status;
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (status.MPI_TAG == TAG_TERMINATE) {
            terminated = true;

            int bestRank;
            MPI_Recv(&bestRank, 1, MPI_INT, 0, TAG_TERMINATE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (bestRank == rank) { // send the best state of all to the master to hand in
                sendBoardState(bestState, 0, TAG_FINAL_RESULT);
            }
        } else {
            int recievedBestValue;
            BoardState task = recieveBoardState(TAG_TASK, recievedBestValue);
            tryUpdateBestValue(recievedBestValue);
            solveStateOMP(task, NUM_OF_FIRST_MOVES);
            MPI_Send(&bestValue, 1, MPI_INT, 0, TAG_RESULT, MPI_COMM_WORLD);
        }
    }
}