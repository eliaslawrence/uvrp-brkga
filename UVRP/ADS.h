#ifndef UVRP_ADS_H_
#define UVRP_ADS_H_

#include <memory>
#include "Evaluation.hpp"

namespace UVRP

{

class ADS { //this class store information to be used by the device ... if the instances are too big (i. e. millions of nodes), you should allocate these pointers into global memory ... otherwise just use constant memory
public:		//also this class has some information to accelerate the evaluation of a solution. If possible (i. e. not that slow to compute), always let your heuristic have this structured atached to the current solution.
	
		
/*	int max_n_trip; //maximum number of trips, also used as the size of trip_cost, trip_size and hotel_index (number of cells allocated)
	int n_trip; //number of trips, also used as the length of trip_cost, trip_size and hotel_index (number of cells used)
	double* trip_cost; //cost of each trip, size is max_n_trip and length n_trip*/
	
	//int* order; //solution into a simple array for the gpu
	//int order_size;	//max size of order array (number of cell allocated)
	//int order_length; //actual size of order array (number of cells used) 
	//int* trip_size; //number of clients in each trip, size is max_n_trip and length is n_trip
	//int* hotel_order; //order of used hotels in the tour, starting and ending hotel isn't tracked; size is max_n_trip and length is n_trip-1
	//int* trip_index; //what trip that element belongs to relative to int* order index, size is order_size and length is order_length; example: c = 5 h = 2 n = 7  order:       3 0 2 5 4 6 1  <--translates to--> start at hotel 5, visit clients 3 0 2 in a trip, stop at hotel 5, visit client 4, stop at hotel 6, visit client 1, end the tour at hotel 5   
					  //																							      	                                      client_trip: 0 0 0 1 1 1 2  <--translates to--> index 0 to 2 belongs to the first trip ... index 3 to 5 belongs to the second trip ... index 6 belongs to last trip
					  //note that hotel 'i' belongs to two trips  (index_trip[i]-1 and index_trip[i])


    std::unique_ptr<UVRP_Evaluation> evaluation;

	ADS(/*int size = 0, int max = 0*/) = default;
	~ADS() = default;
	ADS(const ADS&);
	ADS(ADS&&);

	void swap(ADS&);
	void clear();

	ADS& operator=(ADS);
	ADS& operator=(ADS&&);

	friend std::ostream& operator<<(std::ostream&, const ADS&);
};

std::ostream& operator<<(std::ostream&, const ADS&);

}



#endif
