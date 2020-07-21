#ifndef _HOTEL_SELECT_HPP_
#define _HOTEL_SELECT_HPP_

#include "./ProblemInstance.h"
#include "Solution.hpp"

namespace UVRP
{

class URVP {
protected:
    const ProblemInstance& pUVRP;

public:	
    URVP(const ProblemInstance& _pUVRP) : pUVRP(_pUVRP) { }
    virtual ~URVP() = default;

    virtual UVRP_Solution& operator()(UVRP_Solution&) const = 0;
};

}

#endif
