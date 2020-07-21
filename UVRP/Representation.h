#ifndef UVRP_REPRESENTATION_H_
#define UVRP_REPRESENTATION_H_

#include <iostream>
#include <list>
#include <vector>

#include "Common.h"
#include "ProblemInstance.h"

namespace UVRP

{

class Representation {
public:
//    std::list<unsigned> directions;    //directions as a double linked list

    std::vector<UVRP_Points> pos;    //positions as a double linked list
    std::vector<UVRP_Points> axis;    //positions as a double linked list

    std::vector<std::list<float>>  vels;          //velocities as a double linked list
    std::vector<std::list<float>>  recharge_rates;//recharge rate as a double linked list

    std::vector<unsigned> finalPos;

//	Representation() = default;
    Representation(const UVRP::ProblemInstance& _pUVRP);

	friend std::ostream& operator<<(std::ostream&, const Representation&);
};

std::ostream& operator<<(std::ostream&, const Representation&);

}


#endif /*UVRP_REPRESENTATION_H_*/

