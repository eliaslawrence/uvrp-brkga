#ifndef A_STAR_H_
#define A_STAR_H_

#include <vector>
#include <random>
#include <iomanip>
#include <list>
#include <utility>

#include "Common.h"
#include "AStarNode.h"
#include "Solution.hpp"
#include "BRKGA/Common.h"
#include "ProblemInstance.h"

namespace UVRP
{

class AStar {

private:
    bool preProcessing;

public:
    ProblemInstance& pUVRP;
    AStar(ProblemInstance& _pUVRP, bool _preProcessing);

    UVRP_Points      path              (int start_x, int start_y, int end_x, int end_y) const;
    UVRP_Points      from_file_route   (string file_name) const; /* returns a route from file */
    UVRP_Points      a_star            (int start_x, int start_y, int end_x, int end_y) const;
    BRKGA_RandomKeys encode            (UVRP_Points path) const;
    BRKGA_RandomKeys generateIndividual() const;

};

}

#endif /*A_STAR_H_*/

