#include <algorithm>
#include <numeric>
#include <list>

#include "Decoder.h"

#ifndef NDEBUG
#define DC_DEBUG 1 //comment to hide debugging lines
#endif

#ifdef DC_DEBUG 
#define DEBUG
#else
#define DEBUG while(false)
#endif

using namespace BRKGA;

Decoder::Decoder(const UVRP::ProblemInstance& _pUVRP, const UVRP::AStar& _astar) : pUVRP(_pUVRP), aStar(_astar) {
}

BRKGA_Individual Decoder::operator()(BRKGA_RandomKeys& random_keys, int phase) const {
        return decode(random_keys, phase);
}

BRKGA_Chromossome Decoder::getRepresentation(BRKGA_RandomKeys& random_keys) const {
//    assert(random_keys.size() == 60);

    BRKGA_Chromossome representation(pUVRP);

    auto get_vel = [&](const double& a)->float{
        return (float) a * pUVRP.max_vel;
    };


    for (int curr_vehicle = 0; curr_vehicle < pUVRP.v; ++curr_vehicle) {
        std::vector<unsigned> rep(random_keys.size() / (3 * pUVRP.v));
        std::iota(rep.begin(), rep.end(), 0);

        for (int i = 0; i < rep.size(); i++) {
            if(getX(pUVRP.specialPoints[i]) == pUVRP.origin_x &&
               getY(pUVRP.specialPoints[i]) == pUVRP.origin_y){
                rep.erase(rep.begin() + i);
                break;
            }
        }

        auto compare = [&](const int& a, const int& b)->bool{
            return random_keys[a + rep.size() * curr_vehicle] < random_keys[b + rep.size() * curr_vehicle]; //a*3 + 2
        };

        std::sort(rep.begin(), rep.end(), compare);

        representation.pos[curr_vehicle].emplace_back(std::make_pair(pUVRP.origin_x, pUVRP.origin_y));
    //    representation.vels.emplace_back(pUVRP.max_vel);
    //    representation.recharge_rates.emplace_back(1);

        int ind, pos;
        for (ind = 0; ind < rep.size(); ind++) {
            pos = rep[ind];

            representation.axis[curr_vehicle].emplace_back(pUVRP.specialPoints[pos]);

            representation.pos[curr_vehicle].emplace_back(pUVRP.specialPoints[pos]);
    //        representation.vels.emplace_back(pUVRP.max_vel);
    //        representation.recharge_rates.emplace_back(1);
            representation.vels[curr_vehicle].emplace_back(get_vel(random_keys[rep.size() * pUVRP.v + rep.size() * curr_vehicle + ind]));
            representation.recharge_rates[curr_vehicle].emplace_back((float) random_keys[2 * rep.size() * pUVRP.v + rep.size() * curr_vehicle + ind]);
        }
    }


    DEBUG{
        std::cerr << "Decoded: ";
        for (int var = 0; var < representation.pos.size(); ++var) {
            for(std::pair<int, int>& p : representation.pos[var]) std::cerr << "(" << getX(p) << ", " << getY(p) << ") ";
            std::cerr << '\n' ;
        }
        std::cerr << "\n";

//        for(float& x : representation.vels) std::cerr << x << " ";
//        std::cerr << "\n";

//        for(float& x : representation.recharge_rates) std::cerr << x << " ";
//        std::cerr << "\n";
    }

    return representation;
}

BRKGA_Chromossome Decoder::getRepresentationPhase2(BRKGA_RandomKeys& random_keys) const {
//    assert(random_keys.size() == 60);
	
    BRKGA_Chromossome representation(pUVRP);

    auto get_vel = [&](const double& a)->float{
        return (float) a * pUVRP.max_vel;
    };

    for (int curr_vehicle = 0; curr_vehicle < pUVRP.v; ++curr_vehicle) {
        std::vector<unsigned> rep(random_keys.size() / (3 * pUVRP.v));
        std::iota(rep.begin(), rep.end(), 0);

        for (int i = 0; i < rep.size(); i++) {
            if(getX(pUVRP.specialPoints[i]) == pUVRP.origin_x &&
               getY(pUVRP.specialPoints[i]) == pUVRP.origin_y){
                rep.erase(rep.begin() + i);
                break;
            }
        }

        auto compare = [&](const int& a, const int& b)->bool{
            return random_keys[a + rep.size() * curr_vehicle] < random_keys[b + rep.size() * curr_vehicle]; //a*3 + 2
        };

        std::sort(rep.begin(), rep.end(), compare);

        int start_x = pUVRP.origin_x;
        int start_y = pUVRP.origin_y;

        int end_x, end_y;

        UVRP_Points subPath;
        representation.pos[curr_vehicle].emplace_back(std::make_pair(pUVRP.origin_x, pUVRP.origin_y));
    //    representation.vels.emplace_back(pUVRP.max_vel);
    //    representation.recharge_rates.emplace_back(1);

        int ind, pos, i = 0;
        for (ind = 0; ind < rep.size(); ind++) {
            pos = rep[ind];

            representation.axis[curr_vehicle].emplace_back(pUVRP.specialPoints[pos]);

            end_x = getX(pUVRP.specialPoints[pos]);
            end_y = getY(pUVRP.specialPoints[pos]);

            subPath = aStar.path(start_x, start_y, end_x, end_y);

            representation.pos[curr_vehicle].insert(representation.pos[curr_vehicle].end(), subPath.begin() + 1, subPath.end());

    //        representation.vels.insert(representation.vels.end(), subPath.size() - 1, pUVRP.max_vel);
    //        representation.recharge_rates.insert(representation.recharge_rates.end(), subPath.size() - 1, 1);
            representation.vels[curr_vehicle].insert(representation.vels[curr_vehicle].end(), subPath.size() - 1, get_vel(random_keys[rep.size() * pUVRP.v + rep.size() * curr_vehicle + ind]));//pUVRP.max_vel);//
            representation.recharge_rates[curr_vehicle].insert(representation.recharge_rates[curr_vehicle].end(), subPath.size() - 1, (float) random_keys[2 * rep.size() * pUVRP.v + rep.size() * curr_vehicle + ind]);//1);//

            start_x = end_x;
            start_y = end_y;
            i++;
        }
    }

	DEBUG{
		std::cerr << "Decoded: "; 
        for (int var = 0; var < representation.pos.size(); ++var) {
            for(std::pair<int, int>& p : representation.pos[var]) std::cerr << "(" << getX(p) << ", " << getY(p) << ") ";
        }
		std::cerr << "\n";		

//        for(float& x : representation.vels) std::cerr << x << " ";
//        std::cerr << "\n";

//        for(float& x : representation.recharge_rates) std::cerr << x << " ";
//        std::cerr << "\n";
	}

	return representation;
}

BRKGA_Individual Decoder::decode(BRKGA_RandomKeys& random_keys, int phase) const {
    assert(random_keys.size() != 0);

	DEBUG{
		std::cerr << "Decoding: "; 
        std::cerr << "(" << random_keys.size() << ") ";
		for(const double& x : random_keys) 
			std::cerr << x << " ";
		std::cerr << "\n";		
	}

    if(phase == 1){
        BRKGA_Individual individual(getRepresentation(random_keys), UVRP::ADS()); //sort the rk to get a dna
        return individual;
    }

    BRKGA_Individual individual(getRepresentationPhase2(random_keys), UVRP::ADS()); //sort the rk to get a dna

//    return uvrp(individual); //getting full solution with hotel selection component
    return individual;
}

