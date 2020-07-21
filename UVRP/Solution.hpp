#ifndef UVRP_SOLUTION_H_
#define UVRP_SOLUTION_H_

#include <cmath>
#include "../../OptFrame/Solution.hpp"
#include "Representation.h"
#include "ADS.h"

//if you change the representation by reusing the same solution variable, then clear ADS using clear()
using UVRP_Solution = optframe::Solution<UVRP::Representation, UVRP::ADS>;

namespace optframe
{

inline std::ostream& operator<<(ostream& os, const UVRP_Solution& sol){
	os << sol.getADS() << std::endl;
	os << sol.getR();
	return os;
}

}

#endif
