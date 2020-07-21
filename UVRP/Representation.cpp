#include <ostream>
#include <algorithm>

#include "Representation.h"

#ifndef NDEBUG
#define RP_DEBUG 1 //uncomment to activate debug mode
#endif

#ifdef RP_DEBUG
#define DEBUG
#else
#define DEBUG while(false)
#endif

UVRP::Representation::Representation(const UVRP::ProblemInstance &_pUVRP) : pos(), axis(), vels(), recharge_rates(){
    finalPos.assign(_pUVRP.v, 0);

    pos.assign(_pUVRP.v, UVRP_Points());
    axis.assign(_pUVRP.v, UVRP_Points());

    vels.assign(_pUVRP.v, std::list<float>());
    recharge_rates.assign(_pUVRP.v, std::list<float>());

//    for_each(pos.begin(), pos.end(), [this](std::vector<int> &a){
//        a.resize(2); std::fill(a.begin(), a.end(), 0);
//    });
}

std::ostream& UVRP::operator<<(std::ostream &s, const UVRP::Representation& rep){
//    s << "Directions: ";
//    for(const auto& x : rep.directions) s << x << ' ';

//    s << "\nAXIS: ";
//    for (int i = 0; i < rep.axis.size(); ++i) {
//        s << '(' << getX(rep.axis[i]) << ',' << getY(rep.axis[i]) << ") ";
//    }

//    s << "\nPOS: ";
//    for (int i = 0; i < rep.pos.size(); ++i) {
//        s << '(' << getX(rep.pos[i]) << ',' << getY(rep.pos[i]) << ") ";
//    }

    s << "\nVelocities: ";
    for (int var = 0; var < rep.vels.size(); ++var) {
        for(const auto& x : rep.vels[var]) s << x << ' ';
        std::cerr << '\n' ;
    }

    s << "\nCharge Rates: ";
    for (int var = 0; var < rep.recharge_rates.size(); ++var) {
        for(const auto& x : rep.recharge_rates[var]) s << x << ' ';
        std::cerr << '\n' ;
    }

	return s;
}

