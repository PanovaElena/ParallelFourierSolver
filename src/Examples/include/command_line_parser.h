#pragma once
#include "mpi_wrapper_3d.h"
#include <cmath>
#include <map>
#include <string>
#include <iostream>
#include "parameters_for_test.h"
#include "status.h"

enum Task {
    consistent,
    parallel
};

class CommandLineParser {
protected:
    std::map<std::string, std::string> m;
    MPIWrapper3d mpiWrapper = MPIWrapper3d();

public:
    virtual void help(ParametersForTest& p, Task task) = 0;
    virtual Status saveArgs(ParametersForTest& p, Task task) = 0;

    Status parseArgsForConsistent(int& argc, char**& argv, ParametersForTest& p) {
        return checkArgs(argc, argv, p, Task::consistent);
    }

    Status parseArgsForParallel(int& argc, char**& argv, ParametersForTest& p, MPIWrapper3d& mw) {
        mpiWrapper.SetSize(MPIWrapper::MPISize(), 1, 1);
        Status s = checkArgs(argc, argv, p, Task::parallel);
        mw = mpiWrapper;
        return s;
    }

protected:
    Status parseArgs(int& argc, char**& argv, ParametersForTest& p, Task task) {
        if (argc > 1 && std::string(argv[1]) == "-help") {
            help(p, task);
            return Status::STOP;
        }
        else {
            for (int i = 1; i < argc && i + 1 < argc; i += 2)
                m.emplace(argv[i], argv[i + 1]);
        }
        return Status::OK;
    }

    Status checkArgs(int& argc, char**& argv, ParametersForTest& p, Task task) {
        Status res = parseArgs(argc, argv, p, task);
        if (res != 0) return res;
        return saveArgs(p, task);
    }
};