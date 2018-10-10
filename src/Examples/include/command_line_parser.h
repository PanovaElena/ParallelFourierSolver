#pragma once
#include <map>
#include <string>
#include <iostream>
#include "parameters_for_test.h"

class CommandLineParser {
protected:
    std::map<std::string, std::string> m;
public:

    int parseArgs(int& argc, char**& argv) {
        if (argc > 1 && std::string(argv[1]) == "--help") {
            help();
            return 1;
        }
        else {
            for (int i = 1; i < argc && i + 1 < argc; i += 2)
                m.emplace(argv[i], argv[i + 1]);
        }
        return 0;
    }

    int parseArgs(int& argc, char**& argv, ParametersForTest& p) {
        int res;
        res = parseArgs(argc, argv);
        if (res != 0) return res;
        saveArgs(p);
        return 0;
    }

    virtual void help() = 0;
    virtual void saveArgs(ParametersForTest& p) = 0;
};