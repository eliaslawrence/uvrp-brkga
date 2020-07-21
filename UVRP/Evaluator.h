#ifndef UVRP_EVALUATOR_H_
#define UVRP_EVALUATOR_H_

#include <vector>

#include "Solution.hpp"
#include "ProblemInstance.h"

namespace UVRP
{

class Evaluator {
public:
    const ProblemInstance& pUVRP;
    Evaluator(const ProblemInstance& _pUVRP);

    const UVRP_Evaluation& evaluate(UVRP_Solution&) const; //returns the quality of the solution
    bool is_feasible(const UVRP_Solution&) const; //checks if the solution is feasible. It is expected an evaluated solution as input.
    bool is_valid(const UVRP_Solution&) const; //checks if the solution is valid. It is expected an evaluated solution as input.

    const UVRP_Evaluation& operator()(UVRP_Solution&) const;
};

}

#endif /*UVRP_EVALUATOR_H_*/

