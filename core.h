#ifndef CORE_H
#define CORE_H

#include <string>
#include <iostream>
#include <random>
#include <chrono>
#include <sstream>

#include "UVRP/ProblemInstance.h"
#include "UVRP/Solution.hpp"
#include "UVRP/Evaluator.h"
#include "UVRP/UVRP.hpp"
#include "BRKGA/BRKGA.h"
#include "BRKGA/Decoder.h"

class Core
{
private:
    int argc;
    char **argv;

    void usage(const char* name) const;
public:
    Core() = default;
    Core(int argc, char** argv);
    ~Core() = default;

    void run_BRKGA(int argc, char** argv);
    std::string format(UVRP::Representation);

    void operator()();
};

#endif // CORE_H
