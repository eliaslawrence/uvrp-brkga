#ifndef _UVRP_EVALUATION_HPP_
#define _UVRP_EVALUATION_HPP_

#include <tuple>
#include <iostream>
#include <iomanip>

//using UVRP_Evaluation = std::tuple<unsigned, unsigned, double>; //number of trips, total travel time and number of invalid trips
//time, consumption, final charge, num clients, invalid points, min charge

using UVRP_Evaluation = std::tuple<unsigned, unsigned, unsigned, unsigned, double, double, double>;
//double&   time              = 0;
//double&   consumption       = 0;
//double&   final_charge      = 0;
//unsigned& num_invCharge     = 0;
//unsigned& num_clients       = 0;
//unsigned& prohibited_points = 0;
//unsigned& out_of_bounds     = 0;

//#define getTripCount std::get<0>
//#define getInvTripCount std::get<1>
//#define getTT std::get<2>

#define getOutOfBoundsCount std::get<0>
#define getProhibitedCount  std::get<1>
#define getClientsCount     std::get<2>
#define getInvChargeCount   std::get<3>
#define getFinalCharge      std::get<4>
#define getConsumption      std::get<5>
#define getTime             std::get<6>

//#define PRINT




namespace UVRP
{

//namelookup being anoying, ugly solution but works
inline std::ostream& print_evaluation(std::ostream& os, const UVRP_Evaluation& evaluation){
	#ifdef PRINT
    os << "#Clients - ";
	#endif
    os << getClientsCount(evaluation) << "\t";
	#ifdef PRINT
    os << "#TotalTime - ";
	#endif	
    os << std::fixed << std::setprecision(1) << getTime(evaluation) << "\t";
    #ifdef PRINT
    os << "#Consumption - ";
    #endif
    os << std::fixed << std::setprecision(1) << getConsumption(evaluation) << "\t";
    #ifdef PRINT
    os << "#FinalCharge - ";
    #endif
    os << std::fixed << std::setprecision(1) << getFinalCharge(evaluation) << "\t";
	#ifdef PRINT
    os << "#InvalidCharge - ";
	#endif
    os << getInvChargeCount(evaluation) << "\t";
    #ifdef PRINT
    os << "#OutOfBounds - ";
    #endif
    os << getOutOfBoundsCount(evaluation) << "\t";
    #ifdef PRINT
    os << "#ProhibitedPoints - ";
    #endif
    os << getProhibitedCount(evaluation);
	return os;
}

//is a better than b?
inline bool betterThan(const UVRP_Evaluation& a, const UVRP_Evaluation& b){
//    return getTime(a) < getTime(b);

    if(getOutOfBoundsCount(a) != getOutOfBoundsCount(b)){
         return getOutOfBoundsCount(a) < getOutOfBoundsCount(b);
    }else{
        if(getProhibitedCount(a) != getProhibitedCount(b)){
            return getProhibitedCount(a) < getProhibitedCount(b);
        } else {
            if(getInvChargeCount(a) != getInvChargeCount(b)){
                return getInvChargeCount(a) < getInvChargeCount(b);
            } else {
                if(getClientsCount(a) != getClientsCount(b)){
                    return getClientsCount(a) > getClientsCount(b);
                } else {
                    return getTime(a) + getConsumption(a) - 5*getFinalCharge(a) < getTime(b) + getConsumption(b) - 5*getFinalCharge(b);
                }
            }
        }
    }

//    if(getOutOfBoundsCount(a) != getOutOfBoundsCount(b)){
//         return getOutOfBoundsCount(a) < getOutOfBoundsCount(b);
//    }else{
//        if(getProhibitedCount(a) != getProhibitedCount(b)){
//            return getProhibitedCount(a) < getProhibitedCount(b);
//        } else {
//            if(getInvChargeCount(a) != getInvChargeCount(b)){
//                return getInvChargeCount(a) < getInvChargeCount(b);
//            } else {
//                if(getClientsCount(a) != getClientsCount(b)){
//                    return getClientsCount(a) > getClientsCount(b);
//                } else {
//                    return getTime(a)        <= getTime(b)             &&
//                           getConsumption(a) <= getConsumption(b)      &&
//                           getFinalCharge(a) >= getFinalCharge(b);
//                }
//            }
//        }
//    }

//    return getOutOfBoundsCount(a) <= getOutOfBoundsCount(b) &&
//           getProhibitedCount(a)  <= getProhibitedCount(b)  &&
//           getInvChargeCount(a)   <= getInvChargeCount(b)   &&
//           getClientsCount(a)     >= getClientsCount(b)     &&
//           getTime(a)             <= getTime(b)             &&
//           getConsumption(a)      <= getConsumption(b)      &&
//           getFinalCharge(a)      >= getFinalCharge(b);

    if(getOutOfBoundsCount(a) != getOutOfBoundsCount(b)){
         return getOutOfBoundsCount(a) < getOutOfBoundsCount(b);
    }else{
        if(getProhibitedCount(a) != getProhibitedCount(b)){
            return getProhibitedCount(a) < getProhibitedCount(b);
        } else {
            if(getInvChargeCount(a) != getInvChargeCount(b)){
                return getInvChargeCount(a) < getInvChargeCount(b);
            } else {
                if(getClientsCount(a) != getClientsCount(b)){
                    return getClientsCount(a) > getClientsCount(b);
                } else {
                    if(getTime(a) != getTime(b)){
                        return getTime(a) < getTime(b);
                    } else {
                        if(getConsumption(a) != getConsumption(b)){
                            return getConsumption(a) < getConsumption(b);
                        } else {
                            return getFinalCharge(a) > getFinalCharge(b);
                        }
                    }
                }
            }
        }
    }
}

inline bool is_feasible(const UVRP_Evaluation& a){
    return getOutOfBoundsCount(a) == 0 &&
           getProhibitedCount(a)  == 0 &&
           getInvChargeCount(a)   == 0;
}

//inline double to_double(const UVRP_Evaluation& a){
//	constexpr double BIG_M = 1e15;
//	constexpr double MED_M = 1e10;

//	return getTT(a) + getTripCount(a)*MED_M + getInvTripCount(a)*BIG_M;
//}

//this print evaluation as N_TRIP TOTAl - TRAVEL TIME - INVALID TRIPS
inline std::ostream& operator<<(std::ostream& os, const UVRP_Evaluation& evaluation){
	return print_evaluation(os, evaluation);
}

}

//this print evaluation as N_TRIP TOTAl - TRAVEL TIME - INVALID TRIPS
inline std::ostream& operator<<(std::ostream& os, const UVRP_Evaluation& evaluation){
    return UVRP::print_evaluation(os, evaluation);
}

#endif
