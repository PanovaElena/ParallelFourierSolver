#pragma once
#include <string>
#include "file_writer.h"

class TestParallel {
public:

    std::string nameFileFirstSteps;
    std::string nameFileSecondSteps;

    std::string nameFileAfterDivision;
    std::string nameFileAfterExchange;

    virtual void TestBody() = 0;

    void SetNameFiles() {
        nameFileFirstSteps = "first_sequential_steps.csv";
        nameFileSecondSteps = "parallel_result.csv";
        nameFileAfterDivision = "after_division_into_domains_rank_" + std::to_string(MPIWrapper::MPIRank()) + ".csv";
        nameFileAfterExchange = "after_last_exchange_rank_" + std::to_string(MPIWrapper::MPIRank()) + ".csv";
    }

};
