#include "AStarNode.h"

using namespace UVRP;

AStarNode::AStarNode(ProblemInstance& _pUVRP, AStarNode * _parent, int _x, int _y) :
    pUVRP(_pUVRP), pnt(_x, _y)
{
    parent = _parent;
//    pnt(_x, _y);
//    x = _x;
//    y = _y;
}

std::vector<AStarNode *> AStarNode::adjacents(bool diagonal_movement) /* returns a vector of adjacent valid points */
{
    // Inicializando o array final
    std::vector<AStarNode *> adjacents;

    // Variavel temporaria para guardar os pontos adjacentes
    int adjacent_x = 0,
        adjacent_y = 0;

    int min_hor = -1;
    int max_hor = 1;
    int min_ver = -1;
    int max_ver = 1;

    auto is_valid_x = [&](const int& x)->bool{
        if(x < 0){
            return false;
        }

        if(x >= pUVRP.col){
            return false;
        }

        return true;
    };

    auto is_valid_y = [&](const int& y)->bool{
        if(y < 0){
            return false;
        }

        if(y >= pUVRP.lines){
            return false;
        }

        return true;
    };

    // Percorre na vertical
    for (int i = min_ver; i <= max_ver; i++)
    {
        // Percorre na horizontal
        for (int j = min_hor; j <= max_hor; j++)
        {
            // Not reference point
            if(i != 0 || j != 0){

                // Reference point added loop value
                adjacent_x = pnt.x + j;
                adjacent_y = pnt.y + i;

                // Inside matrix
                if (is_valid_x(adjacent_x) && is_valid_y(adjacent_y)) {

                    // Valid movement
                    if ((!diagonal_movement && (i == 0 || j == 0)) || (diagonal_movement)) {

                        // Not prohibited
                        if(pUVRP.grid[adjacent_y][adjacent_x] != POINTS::PROHIBITED){

                            // Add node to vector
                            adjacents.emplace_back(new AStarNode(pUVRP, this, adjacent_x, adjacent_y));
                        }
                    }
                }
            }
        }
    }

    return adjacents;
}

bool UVRP::operator==(AStarNode& a, AStarNode& b){
    return a.pnt == b.pnt;
}
