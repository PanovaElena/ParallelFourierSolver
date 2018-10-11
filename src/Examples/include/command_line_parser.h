#pragma once
#include "mpi_wrapper_3d.h"
#include <cmath>
#include <map>
#include <string>
#include <iostream>
#include "parameters_for_test.h"

enum Task {
    consistent,
    parallel
};

class CommandLineParser {
protected:
    std::map<std::string, std::string> m;
    MPIWrapper3d mpiWrapper = MPIWrapper3d();

public:
    virtual void help(Task task) = 0;
    virtual int saveArgs(ParametersForTest& p, Task task) = 0;

    int parseArgsForConsistent(int& argc, char**& argv, ParametersForTest& p) {
        return checkArgs(argc, argv, p, Task::consistent);
    }

    int parseArgsForParallel(int& argc, char**& argv, ParametersForTest& p, MPIWrapper3d& mw) {
        int pw = (int)(pow(MPIWrapper::MPISize(), 1.0 / 3) + 0.5);
        mpiWrapper.SetSize(pw, pw, pw);
        int res = checkArgs(argc, argv, p, Task::parallel);
        mw = mpiWrapper;
        return res;
    }

private:
    int parseArgs(int& argc, char**& argv, Task task) {
        if (argc > 1 && std::string(argv[1]) == "--help") {
            help(task);
            return 2;
        }
        else {
            for (int i = 1; i < argc && i + 1 < argc; i += 2)
                m.emplace(argv[i], argv[i + 1]);
        }
        return 0;
    }

    int checkArgs(int& argc, char**& argv, ParametersForTest& p, Task task) {
        int res;
        res = parseArgs(argc, argv, task);
        if (res != 0) return res;
        return saveArgs(p, task);
    }
};