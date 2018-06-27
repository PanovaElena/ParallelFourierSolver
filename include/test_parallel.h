#pragma once
#include <string>
#include "mpi_worker.h"
#include "write_file.h"

class TestParallel {
public:

    MPIWorker worker;
    std::string dir;

    std::string nameFileFirstSteps;
    std::string nameFileSecondSteps;

    std::string nameFileStartParallel;
    std::string nameFileFinalParallel;
    std::string nameFileAfterExchange;


    std::string arrNameFileFinalParallelSteps[100];
    std::string arrNameFileStartParallelSteps[100];

    virtual void TestBody() = 0;

    void SetNameFiles(std::string _dir) {
        dir = _dir;
        nameFileFirstSteps = dir + "first_steps.csv";
        nameFileSecondSteps = dir + "second_steps.csv";
        nameFileStartParallel = dir + "parallel_steps_start_";
        nameFileFinalParallel = dir + "parallel_steps_final_";
        nameFileAfterExchange = dir + "after_exchange_rank_"+ 
            std::to_string(MPIWrapper::MPIRank())+".csv";
        InitNameFiles();
    }

    void InitNameFiles() {
        for (int i = 0; i < MPIWrapper::MPISize(); i++) {
            arrNameFileFinalParallelSteps[i] = nameFileFinalParallel + std::to_string(i) + ".csv";
            arrNameFileStartParallelSteps[i] = nameFileStartParallel + std::to_string(i) + ".csv";
        }
    }

};
