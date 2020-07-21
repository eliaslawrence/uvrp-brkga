#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iomanip>
#include "ProblemInstance.h"

#ifndef NDEBUG
#define PI_DEBUG 1 //uncomment to switch to debug mode
#endif

#ifdef PI_DEBUG
#define DEBUG
#else
#define DEBUG while(false)
#endif

using namespace UVRP;

ProblemInstance::ProblemInstance(int _seed, std::string _directoryName) : seed (_seed), directory_name (_directoryName), pseudorandom_engine(_seed){
	struct Point{
		float x, y;
    };    

    std::ifstream matrixFile;

    std::string matrixFileName = directory_name + "matrix.txt";
    matrixFile.open(matrixFileName);

    setOriginPoint(directory_name + "origin.txt");

    if ( matrixFile ){
        file_name = matrixFileName;
        setGraph();
    } else {
        std::ifstream gridFile(file_name.c_str());

        assert(gridFile);

        std::string temp;
        std::getline(gridFile, temp); // Names
        std::getline(gridFile, temp); // Comment
        std::getline(gridFile, temp); // Type

        gridFile >> temp; //Dimension
        gridFile >> temp; // :

        int maxX = std::numeric_limits<int>::min(), minX = std::numeric_limits<int>::max(),
            maxY = std::numeric_limits<int>::min(), minY = std::numeric_limits<int>::max();

        gridFile >> c; //Dimension

        std::getline(gridFile, temp);
        std::getline(gridFile, temp); // Edge Type
        std::getline(gridFile, temp); // Name Coord


        //Clients
        std::vector<Point> c_loc;
        for(int i = 0; i < c; ++i){
            int h_id;
            Point pnt;

            gridFile >> h_id;
            gridFile >> pnt.x;
            gridFile >> pnt.y;

            c_loc.emplace_back(std::move(pnt));

            if(pnt.x > maxX){
                maxX = pnt.x;
            }

            if(pnt.x < minX){
                minX = pnt.x;
            }

            if(pnt.y > maxY){
                maxY = pnt.y;
            }

            if(pnt.y < minY){
                minY = pnt.y;
            }
        }

        gridFile.close();

        col   = maxX - minX + 1;
        lines = maxY - minY + 1;

        //do the graph
        grid.resize(lines);
        for_each(grid.begin(), grid.end(), [this](std::vector<unsigned> &a){ a.resize(col); std::fill(a.begin(), a.end(), 0); });

        for(auto& client : c_loc){
            client.x -= minX;
            client.y -= minY;

            grid[client.y][client.x] = UVRP::POINTS::DELIVER;
            clients.emplace_back(std::make_pair(client.x, client.y));
            specialPoints.emplace_back(std::make_pair(client.x, client.y));
        }

        assert(c < UVRP::MAX_NODES); //if its bigger then you should reimplement the representation

        int qty_charge     = 0, //1 - Variables that will be incremented as the stopping points are instantiated
            qty_prohibited = 0,

            random_x = 0, //2 - Temporary variable that receives a random value along the loop
            random_y = 0,

            map_size = col * lines,

            total_points = map_size - c; // 3 - Qty of points in the map

         r = (unsigned) (pRecharge   * total_points);
         p = (unsigned) (pProhibited * total_points);

        //Recharge
        while (qty_charge != r){
            random_x = rand() % col;
            random_y = rand() % lines;

            if (grid[random_y][random_x] == POINTS::NOTHING) {
                grid[random_y][random_x] = POINTS::RECHARGE;

                specialPoints.emplace_back(std::make_pair(random_x, random_y));
                qty_charge++;
            }
        }

        //Prohibited
        while (qty_prohibited != p){
            random_x = rand() % col;
            random_y = rand() % lines;

            if ((grid[random_y][random_x] == POINTS::NOTHING) &&
                (random_x != origin_x || random_y != origin_y)) {
                grid[random_y][random_x] = POINTS::PROHIBITED;
                qty_prohibited++;

                prohibited.emplace_back(std::make_pair(random_x, random_y));
            }
        }

        std::ofstream gridFileOUT;
        gridFileOUT.open("/home/elias/Documents/Mestrado/Projeto/instances/matrix.txt");

        for (int j = 0; j < grid.size(); ++j) {
            for (int i = 0; i < grid[j].size(); ++i) {
                gridFileOUT << grid[j][i] << " ";
            }
            gridFileOUT << '\n';
        }

        gridFileOUT.close();
    }

//    c = max_c;
}

void ProblemInstance::setGraph() {

    std::ifstream inGRAPH1(file_name.c_str());
    assert(inGRAPH1);


    std::string temp;
    std::getline(inGRAPH1, temp); // Line
    temp.erase (std::remove(temp.begin(), temp.end(), ' '), temp.end());

    col   = temp.length();
    lines = 0;

    do{
        lines++;
    } while(std::getline(inGRAPH1, temp));

    inGRAPH1.close();

    //Initializing the array that represents the map
    grid.resize(lines);
    for_each(grid.begin(), grid.end(), [this](std::vector<unsigned> &a){ a.resize(col); std::fill(a.begin(), a.end(), 0); });

    std::ifstream inGRAPH2(file_name.c_str());

    int x = 0;

    c = 0;
    r = 0;
    p = 0;


    // Inicializa Matriz
    for ( int i = 0; i < grid.size(); i++ ) {
        for ( int j = 0; j < grid[i].size(); j++ ) {
            inGRAPH2 >> x;

            grid[i][j] = x;

            switch (x) {
            case POINTS::DELIVER:
                c++;
                clients.emplace_back(std::make_pair(j, i));
                specialPoints.emplace_back(std::make_pair(j, i));
                break;
            case POINTS::COLLECT:                
                c++;
                clients.emplace_back(std::make_pair(j, i));
                specialPoints.emplace_back(std::make_pair(j, i));
                break;
            case POINTS::RECHARGE:
                r++;
                specialPoints.emplace_back(std::make_pair(j, i));
                break;
            case POINTS::PROHIBITED:
                p++;
                prohibited.emplace_back(std::make_pair(j, i));
                break;
            }
        }
    }

    inGRAPH2.close();     
}

void ProblemInstance::setOriginPoint(std::string originFileName) /* set origin point of the drones */
{
    std::ifstream originFile(originFileName.c_str());

    if ( originFile ){

        originFile >> origin_x;
        originFile >> origin_y;

        originFile.close();

    }
}

std::ostream& UVRP::operator<<(std::ostream& os, const ProblemInstance& pi){
    DEBUG os << "Number of clients/recharge/prohibited";
    os << pi.c << " " << pi.r << " " << pi.p << std::endl;
    DEBUG os << "Vehicles: ";
    os << pi.v << std::endl;
	os << std::endl;
    DEBUG os << "Map: " << std::endl;
    for(auto& i : pi.grid){
		for(auto& j : i){
            os << j << " ";
//			os << std::fixed << std::setprecision(3) << j << "\t";
		}
		os << std::endl;
	}
	DEBUG os  << "Seed: ";
	os << pi.seed << std::endl;
	return os;
}
