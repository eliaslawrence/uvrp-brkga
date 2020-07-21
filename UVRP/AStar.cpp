#include "AStar.h"

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

AStar::AStar(ProblemInstance& _pUVRP, bool _preProcessing) : pUVRP(_pUVRP){
    preProcessing = _preProcessing;
}

UVRP_Points AStar::path(int start_x, int start_y, int end_x, int end_y) const{
    if(preProcessing){
        string fileName = pUVRP.directory_name + "paths/" +
                          to_string(start_x) + "_" + to_string(start_y) + "_" +
                          to_string(end_x)   + "_" + to_string(end_y)   + ".txt";

        ifstream array_file(fileName);

        if(array_file){
            array_file.close();
            return from_file_route(fileName);
        }
    }

    return a_star(start_x, start_y, end_x, end_y);
}

UVRP_Points AStar::from_file_route(string file_name) const /* returns a route from file */
{

    ifstream array_file(file_name);

    int iteration, qty_charge, qty_collect, qty_deliver, num_objectives, size, valid;
    double z;

    array_file >> size;
    array_file >> num_objectives;

    // Inicializando o array final
    UVRP_Points route(size);

    //Iteration
    array_file >> iteration;

    //Valid
    array_file >> valid;

    //Qty Charge
    array_file >> qty_charge;

    //Qty Collect
    array_file >> qty_collect;

    //Qty Deliver
    array_file >> qty_deliver;

    //Objectives
    int i;
//    for (i = 0; i < num_objectives; i++) {
//        array_file >> z;
//    }


    //Solution pairs
    int x, y, v, c;
    double b;

    for (i = 0; i < size; i++) {
        array_file >> x;
        array_file >> y;
        array_file >> v;
        array_file >> c;
        array_file >> b;

        route[i] = std::make_pair(x, y);
    }

    array_file.close();

    return route;
}

UVRP_Points AStar::a_star(int start_x, int start_y, int end_x, int end_y) const /* return the route with a sub path from START point to END modified */
{

    // Create start and end node
    AStarNode * start_node = new AStarNode(pUVRP, NULL, start_x, start_y);
    AStarNode * end_node   = new AStarNode(pUVRP, NULL, end_x  , end_y);

    // Initialize both open and closed list
    vector<AStarNode *> open_list;
    vector<Point> closed_list;

    // Add the start node
    open_list.emplace_back(start_node);

    // Current node
    AStarNode * current_node;
    int current_index;

    // Loop until you find the end
    while(!open_list.empty()){

        // Get the current node of smaller value of f
        current_node = open_list[0];
        current_index = 0;

        for (int index = 0; index < open_list.size(); index++) {
            if(open_list[index]->f < current_node->f){
                current_node = open_list[index];
                current_index = index;
            }
        }

        // Pop current node off open list and add it to the closed list
        open_list.erase(open_list.begin() + current_index);
        closed_list.emplace_back(Point(current_node->pnt.x, current_node->pnt.y));

        // Found the goal
        if(*current_node == *end_node){
            UVRP_Points path;

            while (current_node != NULL) {
                path.emplace(path.begin(), std::make_pair(current_node->pnt.x, current_node->pnt.y));
                current_node = current_node->parent;
            }

            DEBUG std::cerr << "\n";

            // Reverse the path
//            for (int i = temp.size() - 1; i >= 0; i--) {
//                Solution_Pair p = Solution_Pair(temp[i]);
//                path.add_solution_pair(p);
//            }

            // Delete from memory

//            open_list.clear();
//            closed_list.clear();
//            for (int i = 0; i < open_list.size(); i++) {
//                delete open_list[i];
//            }

//            for (int i = 0; i < closed_list.size(); i++) {
//                delete closed_list[i];
//            }

//            temp.clear();

            // Return
            return path;
        }

        // Generate children
        vector<AStarNode *> children = current_node->adjacents(true);

        // Loop through children
        for (AStarNode * child : children) {

            // Child is on the closed list
            for (const Point& closed_child : closed_list) {
                if(child->pnt == closed_child){
                    goto next_iteration;
                }
            }

            // Set f, g and h values
            child->g = current_node->g + 1;
            child->h = pow(child->pnt.x - end_node->pnt.x, 2) + pow(child->pnt.y - end_node->pnt.y, 2);
            child->f = child->g + child->h;

            // Child is already on the open list and better path
            for (AStarNode * open_node : open_list) {
                if(*child == *open_node && child->g > open_node->g){
                    goto next_iteration;
                }
            }

            // Add the child to the open list
            open_list.emplace_back(child);

            next_iteration:;
        }
    }
}

//BRKGA_RandomKeys AStar::encode(UVRP_Points path) const
//{
//    static std::uniform_real_distribution<double> dice(0.0, 1.0);

//    auto get_dir = [](const std::pair<int, int>& p0, const std::pair<int, int>& p1)->double{
//        int delta_x = getX(p1) - getX(p0);
//        int delta_y = getY(p1) - getY(p0);
//        double dir;

//        switch (delta_x) {
//        case -1:
//            switch (delta_y) {
//            case -1:
//                dir = DIRECTIONS::NW;
//                break;
//            case 0:
//                dir = DIRECTIONS::W;
//                break;
//            case 1:
//                dir = DIRECTIONS::SW;
//                break;
//            }
//            break;
//        case 0:
//            switch (delta_y) {
//            case -1:
//                dir = DIRECTIONS::N;
//                break;
//            case 1:
//                dir = DIRECTIONS::S;
//                break;
//            }
//            break;
//        case 1:
//            switch (delta_y) {
//            case -1:
//                dir = DIRECTIONS::NE;
//                break;
//            case 0:
//                dir = DIRECTIONS::E;
//                break;
//            case 1:
//                dir = DIRECTIONS::SE;
//                break;
//            }
//            break;
//        }
//        return dir / (double) DIRECTIONS::COUNT;
//    };

//    BRKGA_RandomKeys individual(3* (path.size() - 1));

//    int ind = 0;
//    for(int i = 0; i < individual.size(); ++i)
//    {
//        individual[i++] = get_dir(path[ind], path[ind+1]);
////        DEBUG std::cerr << individual[i - 1] << " ";

//        individual[i++] = dice(pUVRP.pseudorandom_engine);
//        individual[i]   = dice(pUVRP.pseudorandom_engine);

//        ind++;
//    }


//    return individual;
//}

//BRKGA_RandomKeys AStar::generateIndividual() const
//{
//    UVRP_Points clientsCopy = pUVRP.clients;
//    int randomPos;

//    UVRP_Points path, subPath;
//    path.emplace_back(std::make_pair(pUVRP.origin_x, pUVRP.origin_y));

//    clientsCopy.erase(clientsCopy.begin());

//    for(int i = 1; i < pUVRP.c; ++i)
//    {
//        std::uniform_int_distribution<int> dice (0, clientsCopy.size());
//        randomPos = dice(pUVRP.pseudorandom_engine);

////        DEBUG std::cerr << i << " - " << randomPos << " (" << getX(clientsCopy[randomPos]) << ", " << getY(clientsCopy[randomPos]) << ") ";

//        subPath = a_star(getX(path[path.size() - 1]) , getY(path[path.size() - 1]),
//                         getX(clientsCopy[randomPos]), getY(clientsCopy[randomPos]));

//        path.insert(path.end(), subPath.begin() + 1, subPath.end());

//        clientsCopy.erase(clientsCopy.begin() + randomPos);
//    }

//    return encode(path);
//}
