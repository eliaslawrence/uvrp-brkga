#ifndef UVRP_PROBLEMINSTANCE_H_
#define UVRP_PROBLEMINSTANCE_H_

#include "Common.h"

#include <vector>
#include <random>
#include <string>

namespace UVRP
{

constexpr unsigned MAX_NODES = 5000; //if you have istances bigger then you should use another solution representation && another mean to store the instance

enum POINTS { NOTHING    = 0,
              DELIVER    = 1,
              COLLECT    = 2,
              RECHARGE   = 3,
              PROHIBITED = 4};


enum DIRECTIONS { N  = 0,
                  NE = 1,
                  E  = 2,
                  SE = 3,
                  S  = 4,
                  SW = 5,
                  W  = 6,
                  NW = 7,
                  COUNT = 8};

struct Point {
    int x;
    int y;

    Point(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    bool operator==(const Point& a){
        return a.x == x && a.y == y;
    }
};

class ProblemInstance {
public:
    unsigned c; //number of clients
    unsigned r; //number of recharge points
    unsigned p; //number of prohibited points
    unsigned v = 2; //number of vehicles

    unsigned max_c = 11;

    unsigned origin_x = 0,
             origin_y = 0;

    float pRecharge   = .01; //percentage of recharge points
    float pProhibited = .01; //percentage of prohibited points

    float max_vel        = 10;
    float min_vel        =  1;
    float discretization = 10;

    // CHARGE settings
    float min_recharge_rate      =   0;
    float max_recharge_rate      = 100;
    float max_battery_charge     = 100;
    float initial_battery_charge = 100;
    float fixed_consumption      =  .1;
    float variable_consumption   =  .1;//.05
    float recharge_time          =   5;

    unsigned col;   //number of columns
    unsigned lines; //number of lines
    UVRP_Points clients; //clients positions
    UVRP_Points specialPoints; //clients positions
    UVRP_Points prohibited; //prohibited positions
    std::vector<std::vector<unsigned> > grid; //map
    std::mt19937 pseudorandom_engine; //mersenne twister engine for pseudorandom number generator
    int seed = 0; //seed for deterministic execution

    std::string directory_name;
    std::string file_name = "/home/elias/Documents/Mestrado/uav_routing_qt/UAV_Routing_Qt_1/instances/rat195.tsp";
    std::string test_file_name = "/home/elias/Documents/Mestrado/Projeto/tests/";

    ProblemInstance(int seed = 0, std::string directory_name = "/home/elias/Documents/Mestrado/Projeto/instances/eil_101/");
    void setGraph();
    void setOriginPoint(std::string originFileName); // set origin point of the drones

	friend std::ostream& operator<<(std::ostream&, const ProblemInstance&);
};

std::ostream& operator<<(std::ostream&, const ProblemInstance&);

}

#endif /*UVRP_PROBLEMINSTANCE_H_*/

