#pragma once
#include "mpi_worker.h"
#include "mpi_worker_new.h"
#include <string>
#include "file_writer.h"

class TestParallel {
public:

    MPIWorkerNew worker;

    std::string nameFileFirstSteps;
    std::string nameFileSecondSteps;

    std::string nameFileStartParallel;
    std::string nameFileFinalParallel;
    std::string nameFileAfterExchange;


    std::string arrNameFileFinalParallelSteps[100];
    std::string arrNameFileStartParallelSteps[100];

    virtual void TestBody() = 0;

    void SetNameFiles() {
        nameFileFirstSteps = "first_steps.csv";
        nameFileSecondSteps = "second_steps.csv";
        nameFileStartParallel = "parallel_steps_start_";
        nameFileFinalParallel = "parallel_steps_final_";
        nameFileAfterExchange = "after_exchange_rank_"+ 
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
