#ifndef _BRKGA_DECODER_H_
#define _BRKGA_DECODER_H_

#include "../UVRP/ProblemInstance.h"
#include "../UVRP/Solution.hpp"
#include "../UVRP/UVRP.hpp"
#include "../UVRP/AStar.h"
#include "Common.h"

namespace BRKGA
{

class Decoder {
protected:
    const UVRP::ProblemInstance& pUVRP;
    const UVRP::AStar& aStar;
//    const UVRP::URVP& uvrp;

public:
    Decoder(const UVRP::ProblemInstance& _pUVRP, const UVRP::AStar& _astar);
	~Decoder() = default;

    BRKGA_Chromossome getRepresentation(BRKGA_RandomKeys& random_keys) const; //convert the random keys into values of direction, velocity and recharge rate
    BRKGA_Chromossome getRepresentationPhase2(BRKGA_RandomKeys& random_keys) const; //convert the random keys into values of direction, velocity and recharge rate
    BRKGA_Individual decode(BRKGA_RandomKeys& random_keys, int phase) const;

    BRKGA_Individual operator()(BRKGA_RandomKeys& random_keys, int phase) const;
};

}


#endif
