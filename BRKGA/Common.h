#ifndef _BRKGA_COMMON_H_
#define _BRKGA_COMMON_H_

#include "../UVRP/Solution.hpp"

#include <memory>
#include <vector>
#include <utility>
#include <tuple>

using BRKGA_Chromossome = UVRP::Representation;
using BRKGA_Individual = UVRP_Solution;
using BRKGA_RandomKeys = std::vector<double>;
using BRKGA_Fitness = UVRP_Evaluation; //solution has a fitness inside encapsulated by unique_ptr, nullptr is lack of evaluation
using BRKGA_Population = std::vector< std::tuple<BRKGA_RandomKeys, std::unique_ptr<BRKGA_Individual> > >; //nullptr for individual is lack of decoding

//Apply this to BRKGA_Population elements
#define getRK(a) std::get<0>(a)
#define getInd(a) std::get<1>(a)
#define getFit(a) std::get<1>(a)->getADS().evaluation

#define NDEBUG 1

#endif
