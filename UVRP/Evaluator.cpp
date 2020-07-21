#include <iomanip>

#include "Evaluator.h"

constexpr float EPSILON = 1e-3; //constant to compare float

#ifndef NDEBUG
#define EV_DEBUG 1 //uncomment to debug this code
#endif

#ifdef EV_DEBUG
#define DEBUG
#else
#define DEBUG while(false)
#endif

using namespace UVRP;

Evaluator::Evaluator(const ProblemInstance& _pUVRP) : pUVRP(_pUVRP){
}

const UVRP_Evaluation& Evaluator::evaluate(UVRP_Solution& sol) const {    

    Representation& representation = sol.getR();
	ADS& ads = sol.getADS();

    DEBUG std::cerr << "Evaluating solution with representation:\n" << representation << "\n";
	if(ads.evaluation != nullptr){
        DEBUG std::cerr << "Using evaluation contained in ADS" << "\n";
	    return *ads.evaluation;
	}
	/*    for(int i = 0; i < ads->n_trip; ++i){
	    	trip_cost = ads->trip_cost[i];
	    	objective_function += trip_cost;
            objective_function += (trip_cost > pUVRP.limit) ? BIG_M : 0.0; //penalize invalid solutions
			objective_function += MED_M; //sum a trip to the objective function
	    }
	    return here*/

    UVRP_Evaluation& evaluation = *(ads.evaluation = make_unique<UVRP_Evaluation>(UVRP_Evaluation()));        
    double&   time              = getTime(evaluation);
    double&   consumption       = getConsumption(evaluation);
    double&   final_charge      = getFinalCharge(evaluation);
    unsigned& num_invCharge     = getInvChargeCount(evaluation);
    unsigned& num_clients       = getClientsCount(evaluation);
    unsigned& prohibited_points = getProhibitedCount(evaluation);
    unsigned& out_of_bounds     = getOutOfBoundsCount(evaluation);

    Point pnt(pUVRP.origin_x, pUVRP.origin_y);

    auto update_x = [&](const unsigned& dir, const int& actual_x)->int{
        switch (dir) {
        case DIRECTIONS::N:
            return actual_x;
        case DIRECTIONS::NE:
            return actual_x + 1;
        case DIRECTIONS::E:
            return actual_x + 1;
        case DIRECTIONS::SE:
            return actual_x + 1;
        case DIRECTIONS::S:
            return actual_x;
        case DIRECTIONS::SW:
            return actual_x - 1;
        case DIRECTIONS::W:
            return actual_x - 1;
        case DIRECTIONS::NW:
            return actual_x - 1;
        }

        return actual_x;
    };

    auto is_invalid_x = [&](const int& x)->bool{
        if(x < 0){
            return true;
        }

        if(x >= pUVRP.col){
            return true;
        }

        return false;
    };

    auto update_y = [&](const unsigned& dir, const int& actual_y)->int{
        switch (dir) {
        case DIRECTIONS::N:
            return actual_y - 1;
        case DIRECTIONS::NE:
            return actual_y - 1;
        case DIRECTIONS::E:
            return actual_y;
        case DIRECTIONS::SE:
            return actual_y + 1;
        case DIRECTIONS::S:
            return actual_y + 1;
        case DIRECTIONS::SW:
            return actual_y + 1;
        case DIRECTIONS::W:
            return actual_y;
        case DIRECTIONS::NW:
            return actual_y - 1;
        }

        return actual_y;
    };

    auto is_invalid_y = [&](const int& y)->bool{
        if(y < 0){
            return true;
        }

        if(y >= pUVRP.lines){
            return true;
        }

        return false;
    };

    std::vector<Point> clients_visited;
    num_clients = 0;
    int max_size = 0;
    for (int j = 0; j < pUVRP.v; ++j) {
        if(representation.pos[j].size() > max_size){
            max_size = representation.pos[j].size();
        }
        representation.finalPos[j] = representation.pos[j].size() - 1;
    }

    for (int i = 0; i < max_size; ++i) {
        for (int j = 0; j < pUVRP.v; ++j) {
            if(i < representation.pos[j].size()){
                Point pnt(getX(representation.pos[j][i]),
                          getY(representation.pos[j][i]));

                unsigned type = pUVRP.grid[pnt.y][pnt.x];

                if(((type == POINTS::DELIVER) || (type == POINTS::COLLECT)) &&
                  (std::find_if(clients_visited.begin(), clients_visited.end(), [&pnt](const Point& p) {return p.x == pnt.x && p.y == pnt.y;}) == clients_visited.end())){
                    clients_visited.emplace_back(std::move(pnt));
                    ++num_clients;
                    representation.finalPos[j] = i;
                }

                // Run through all clients
                if(num_clients == pUVRP.c){
                    goto EVALUATION;
                }
            }
        }
    }

    EVALUATION:


//    auto already_visited = [&](const Point& pnt)->bool{
//        std::vector<Point>::const_iterator it = clients_visited.end();
//        if(std::find(clients_visited.begin(), clients_visited.end(), pnt) != it){
//            return true;
//        }

//        return false;
//    };

//    auto compare_pnt = [&](const Point& p)->bool{
//        return p.x == pnt.x && p.y == pnt.y;
//    };

    unsigned type;
    double distance;    

//    ++num_clients;


    time = 0;
    final_charge = 100;

    for (int curr_vehicle = 0; curr_vehicle < pUVRP.v; ++curr_vehicle) {
        std::list<float>::const_iterator it_vel      = representation.vels[curr_vehicle].begin();
        std::list<float>::const_iterator it_recharge = representation.recharge_rates[curr_vehicle].begin();

        double curr_time = 0;
        double curr_battery_charge = pUVRP.initial_battery_charge;
        int it_pos = 0;

        while(it_pos <= representation.finalPos[curr_vehicle]){

            Point startPnt(pnt.x, pnt.y);

            pnt.x = getX(representation.pos[curr_vehicle][it_pos]);
            pnt.y = getY(representation.pos[curr_vehicle][it_pos]);

            //Consumption according to the drone velocity
            double variable_consumption = ((*it_vel) * pUVRP.variable_consumption);
            curr_battery_charge -= variable_consumption;

            //Fixed Consumption
            curr_battery_charge -= pUVRP.fixed_consumption;

            consumption += variable_consumption + pUVRP.fixed_consumption;

            num_invCharge += (curr_battery_charge < 0);

            distance = pUVRP.discretization * (sqrt((pnt.x - startPnt.x) * (pnt.x - startPnt.x) + (pnt.y - startPnt.y) * (pnt.y - startPnt.y)));
            curr_time += (distance) / (*it_vel);

            if(is_invalid_x(pnt.x) || is_invalid_y(pnt.y)){
                ++out_of_bounds;
            } else {
                type = pUVRP.grid[pnt.y][pnt.x];

                switch (type) {
                case POINTS::RECHARGE:
                {
                    double new_battery_charge = curr_battery_charge + (pUVRP.max_battery_charge * (*it_recharge));

                    if (new_battery_charge > pUVRP.max_battery_charge)
                        new_battery_charge = pUVRP.max_battery_charge;

                    curr_time += pUVRP.recharge_time * (*it_recharge); //time spent charging
                    curr_battery_charge = new_battery_charge;
                    break;
                }
                case POINTS::PROHIBITED:
                    ++prohibited_points;
                }
            }

            ++it_pos;
            ++it_vel;
            ++it_recharge;
        }

        // Time of the solution is the maximum time
        if(curr_time > time){
            time = curr_time;
        }

        // Final charge of the solution is the minimum final charge
        if(curr_battery_charge < final_charge){
            final_charge = curr_battery_charge;
        }
    }

//	DEBUG std::cerr << "Evaluation: " << std::fixed << std::setprecision(1) << to_double(evaluation) << "\n";
    DEBUG std::cerr << "Evaluation: " << std::fixed << std::setprecision(1) << evaluation << "\n";

    return evaluation;
}

//A feasible solution is a valid solution that is possible to be applicable to the problem at hand, i. e., doesn't break the internal restrictions imposed by the problem
bool Evaluator::is_feasible(const UVRP_Solution& sol) const {
//	if(!this->is_valid(sol)) return false;
//    else

        return UVRP::is_feasible(*sol.getADS().evaluation);
}

//A valid solution is one that matches its ADS and doesn't break the representation imposed by the problem. It may or may not be feasible though
bool Evaluator::is_valid(const UVRP_Solution& sol) const {
//	DEBUG{
//		std::cerr << "Checking if solution is valid\n";
//		std::cerr << sol << "\n";
//	}

//	const Representation& representation = sol.getR();
//	const ADS& ads = sol.getADS();
//    assert(ads.evaluation != nullptr); //It is expected an evaluated solution
//    UVRP_Evaluation evaluation = *ads.evaluation;

//    std::vector<bool> visited(pUVRP.c, false);
//	double trip_cost = 0.0;
//	double total_cost = 0.0;
//    int prev = pUVRP.c;
//    for(const int& x : representation.directions){
//        assert(x < pUVRP.n); //what did you put inside this representation?
//        trip_cost += pUVRP.graph[prev][x];
//        if(x < pUVRP.c){ //its a client
//            trip_cost +=  pUVRP.visitation_cost[x];
//			if(visited[x]){
//				DEBUG std::cerr << "Client " << x << " visited more than once\n";
//				return false;
//			}
//			visited[x] = true;
//		}
//		else{ //its a hotel
//            if(trip_cost > pUVRP.limit){
//                DEBUG std::cerr << "One trip ending at hotel " << x << " surpasses the limit: " << trip_cost << " | " << pUVRP.limit << "\n";
//				return false;
//			}
//			total_cost += trip_cost;
//			trip_cost = 0; //reset trip cost
//		}
//		prev = x;
//	}

//    trip_cost += pUVRP.graph[prev][pUVRP.c]; //last iteration to ending hotel
//    if(trip_cost > pUVRP.limit){
//        DEBUG std::cerr << "Last trip surpasses the limit: " << trip_cost << " | " << pUVRP.limit << "\n";
//		return false;
//	}
//	total_cost += trip_cost;

//	if((total_cost - getTT(evaluation)) > EPSILON){
//		DEBUG std::cerr << "Actual total cost doesn't coincide with evaluation\n";
//		return false;
//	}
	
//    for(int i = 0; i < pUVRP.c; ++i){
//		if(!visited[i]){
//			DEBUG std::cerr << "Client " << i << " wasn't visited\n";
//			return false;
//		}
//	}

//	DEBUG std::cerr << "Solution is valid" << std::endl;
	return true;
}

const UVRP_Evaluation& Evaluator::operator()(UVRP_Solution& sol) const {
	return evaluate(sol);
}

/*bool MyEvaluator::check_val(const SolutionUVRP &sol){ //checks if the solution is valid
	auto rep = sol.getR();
	float trip_cost = 0;
    vector<bool> visited(pUVRP.c, false);
	bool flag = true;

	#ifdef EV_DEBUG
	cout << "Checking solution " << rep << endl;
	#endif

    int prev = pUVRP.c;
	for(auto it = rep.order.begin(); it != rep.order.end(); ++it){
        trip_cost += pUVRP.graph[prev][*it];
		
        if(*it < pUVRP.c){ //client
			if(visited[*it]){
				#ifdef EV_DEBUG
				cout << "client " << *it << " has been visited more than once" << endl;
				#endif
				flag = false;
			}

            trip_cost += pUVRP.visitation_cost[*it];
			visited[*it] = true;
		}
		else{ //hotel
            if(prev >= pUVRP.c || trip_cost > pUVRP.limit) {
				#ifdef EV_DEBUG
                if(prev >= pUVRP.c) cout << "two or more consecutive hotels" << endl;
                if(trip_cost > pUVRP.limit) cout << "trip doesn't agree with the limit" << endl;
				#endif
				flag = false; //two consecutive hotels or limit reached
			}
			else trip_cost = 0;
		}

		prev = *it;
	}

    if(prev >= pUVRP.c){
		#ifdef EV_DEBUG
		cout << "last vertex is a hotel" << endl;
		#endif
		flag = false;
	}
	else{
        trip_cost += pUVRP.graph[prev][pUVRP.c];
        if(trip_cost > pUVRP.limit){
			cout << "last trip doesn't agree with the limit" << endl;
			flag = false;
		}
	}

	for(auto it = visited.begin(); it != visited.end(); ++it){
		if(!*it){
			#ifdef EV_DEBUG
			cout << "client " << *it << " wasn't visited" << endl;
			#endif
			flag = false;
		}
	}

	return flag;
}*/

/*bool MyEvaluator::check_con(const SolutionUVRP& sol){
	auto& rep = sol.getR();
	auto& ads = sol.getADS();

	#ifdef EV_DEBUG
	cout << "Checking solution consistency." << endl;
	#endif

	if(ads.order_length != rep.order.size()){
		#ifdef EV_DEBUG
		cout << "Different sizes between ADS order and REP order" << endl;
		#endif
		return false;
	}

	auto it = rep.order.begin();
	int n_trip = 0;
	int n_client = 0;
	float total_cost = 0.0;
	float trip_cost = 0.0;
	float ads_total_cost = 0.0;
    int prev = pUVRP.c;
	for(int i = 0; i < ads.order_length; ++i, ++it){
		if(*it != ads.order[i]){
			#ifdef EV_DEBUG
			cout << "ADS order position " << i << " is different than the respective REP order element: " << *it << ' ' << ads.order[i] << endl;
			#endif

			return false;
		}

        trip_cost += pUVRP.graph[prev][*it];

        if(ads.order[i] >= pUVRP.c){
			if(ads.hotel_order[n_trip] != ads.order[i]){
				#ifdef EV_DEBUG
				cout << "Hotel order at pos " << n_trip << " is inconsistent: " << ads.hotel_order[n_trip] << ' ' << ads.order[i] << endl;
				#endif
				return false;
			}

			if(fabs(ads.trip_cost[n_trip] - trip_cost) >= EPSILON){
				#ifdef EV_DEBUG
				cout << "Trip cost at pos " << n_trip << " is inconsistent: " << ads.trip_cost[n_trip] << ' ' << trip_cost << endl;
				#endif
				return false;
			}
			else ads_total_cost += ads.trip_cost[n_trip];

			if(ads.trip_size[n_trip++] != n_client){
				#ifdef EV_DEBUG
				cout << "Trip size at pos " << n_trip-1 << " is inconsistent: " << ads.trip_size[ads.n_trip-1] << ' ' << n_client << endl;
				#endif
				return false;
			}

			total_cost += trip_cost;
			n_client = 0;
			trip_cost = 0;
		}
		else{
            trip_cost += pUVRP.visitation_cost[*it];
			++n_client;
		}

		if(ads.trip_index[i] != n_trip){
			#ifdef EV_DEBUG
			cout << "Trip index at pos " << i << " is inconsistent" << endl;
			#endif
			return false;
		}

		prev = *it;
	}

    trip_cost += pUVRP.graph[prev][pUVRP.c]; //last client to first hotel

	if(fabs(ads.trip_cost[n_trip] - trip_cost) >= EPSILON){
		#ifdef EV_DEBUG
		cout << "Trip cost at pos " << n_trip << " is inconsistent: " << ads.trip_cost[n_trip] << ' ' << trip_cost << endl;
		#endif
		return false;
	}
	else{
		ads_total_cost += ads.trip_cost[n_trip];
		total_cost += trip_cost;

		if(fabs(ads_total_cost - total_cost) >= EPSILON){
			#ifdef EV_DEBUG
			cout << "Trip total cost is inconsistent: " << ads_total_cost << ' ' << total_cost << endl;
			#endif
			return false;
		}
	}

	if(ads.n_trip != ++n_trip){
		#ifdef EV_DEBUG
		cout << "The number of trips is inconsistent: " << ads.n_trip << ' ' << n_trip << endl;
		#endif
		return false;
	}

	return true;
}*/
