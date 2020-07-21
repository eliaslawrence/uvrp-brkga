#include <iostream>
#include <algorithm>
#include <string.h>
#include <iomanip>

#include "ADS.h"

#ifndef NDEBUG
#define AD_DEBUG 1 //uncomment to debug this code
#endif

#ifdef AD_DEBUG
#define DEBUG
#else
#define DEBUG while(false)
#endif

using namespace UVRP;

/*ADS::ADS(int size, int max) : order_size(size), max_n_trip(max), order(size ? new int[size] : nullptr), trip_cost(max ? new double[max] : nullptr), trip_size(max ? new int[max] : nullptr), hotel_order(max ? new int[max] : nullptr), trip_index(size ? new int[size] : nullptr){
	order_length = 0;
	n_trip = 0;
}*/

/*ADS::~ADS(){
	if(order != nullptr) delete[] order;
	if(trip_cost != nullptr) delete[] trip_cost;
	if(trip_size != nullptr) delete[] trip_size;
	if(hotel_order != nullptr) delete[] hotel_order;
	if(trip_index != nullptr) delete[] trip_index;	
}*/

ADS::ADS(const ADS& ads) /*: order_size(ads.order_size), max_n_trip(ads.max_n_trip), order_length(ads.order_length), n_trip(ads.n_trip), order(ads.order_size ? new int[ads.order_size] : nullptr), trip_cost(ads.max_n_trip ? new double[ads.max_n_trip] : nullptr), trip_size(ads.max_n_trip ? new int[ads.max_n_trip] : nullptr), hotel_order(ads.max_n_trip ? new int[ads.max_n_trip] : nullptr), trip_index(ads.order_size ? new int[ads.order_size] : nullptr)*/ {
    evaluation = std::make_unique<UVRP_Evaluation>(UVRP_Evaluation(*ads.evaluation)); //deep copy
	
/* 	if(order != nullptr) 	   memcpy(order, ads.order, sizeof(int)*order_size);
	if(trip_cost != nullptr)   memcpy(trip_cost, ads.trip_cost, sizeof(double)*max_n_trip);
	if(trip_size != nullptr)   memcpy(trip_size, ads.trip_size, sizeof(int)*max_n_trip);
	if(hotel_order != nullptr) memcpy(hotel_order, ads.hotel_order, sizeof(int)*max_n_trip);
	if(trip_index != nullptr)  memcpy(trip_index, ads.trip_index, sizeof(int)*order_size);	*/
}

void ADS::swap(ADS& ads){
	evaluation.swap(ads.evaluation);
/*	std::swap(order, ads.order); 
	std::swap(order_size, ads.order_size); 
	std::swap(order_length, ads.order_length);
	std::swap(max_n_trip, ads.max_n_trip);  
	std::swap(n_trip, ads.n_trip); 
	std::swap(trip_cost, ads.trip_cost); 
	std::swap(trip_size, ads.trip_size); 
	std::swap(hotel_order, ads.hotel_order); 
	std::swap(trip_index, ads.trip_index); 
	*/
}

void ADS::clear(){
	evaluation.reset();
}

ADS::ADS(ADS&& ads) : ADS() {
	swap(ads);
}

ADS& ADS::operator=(ADS ads){
	swap(ads);
	return *this;	
}

ADS& ADS::operator=(ADS&& ads){
	swap(ads);
	return *this;
}

std::ostream& UVRP::operator<<(std::ostream& os, const ADS& ads){
	DEBUG os << "Evaluation: ";
	(ads.evaluation != nullptr) ? os << std::setprecision(3) << *ads.evaluation : os << "nullptr";
	

/*	os << std::endl;
	os << "Oder Size: " << ads.order_size << " Length: " << ads.order_length << " #max_trip " << ads.max_n_trip << " #trip: " << ads.n_trip << " Order: ";
	if(ads.order != nullptr) for(int i = 0; i < ads.order_length; ++i) os << ads.order[i] << ' ';
	else os << " nullptr";
	os << std::endl << " Trip Cost: ";
	if(ads.trip_cost != nullptr) for(int i = 0; i < ads.n_trip; ++i) os << ads.trip_cost[i] << ' ';
	else os << " nullptr";
	os << std::endl << " Trip Size: ";
	if(ads.trip_size != nullptr) for(int i = 0; i < ads.n_trip; ++i) os << ads.trip_size[i] << ' ';
	else os << " nullptr";
	os << std::endl << " Hotel Order: ";
	if(ads.hotel_order != nullptr) for(int i = 0; i < ads.n_trip-1; ++i) os << ads.hotel_order[i] << ' ';
	else os << " nullptr";
	os << std::endl << " Trip Index: ";
	if(ads.trip_index != nullptr) for(int i = 0; i < ads.order_length; ++i) os << ads.trip_index[i] << ' ';
	else os << " nullptr";	
	os << std::endl;
	#else
	os << ads.order_size << ' ' << ads.order_length << ' ' << ads.n_trip << " " << std::endl;
	if(ads.order != nullptr) for(int i = 0; i < ads.order_length; ++i) os << ads.order[i] << ' ';
	else os << " nullptr";
	os << std::endl;
	if(ads.trip_cost != nullptr) for(int i = 0; i < ads.n_trip; ++i) os << ads.trip_cost[i] << ' ';
	else os << " nullptr";
	os << std::endl;
	if(ads.trip_size != nullptr) for(int i = 0; i < ads.n_trip; ++i) os << ads.trip_size[i] << ' ';
	else os << " nullptr";
	os << std::endl;
	if(ads.hotel_order != nullptr) for(int i = 0; i < ads.n_trip-1; ++i) os << ads.hotel_order[i] << ' ';
	else os << " nullptr";
	os << std::endl;
	if(ads.trip_index != nullptr) for(int i = 0; i < ads.order_length; ++i) os << ads.trip_index[i] << ' ';
	else os << " nullptr";	
	os << std::endl;*/
}





