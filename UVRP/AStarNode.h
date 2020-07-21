#ifndef ASTARNODE_H
#define ASTARNODE_H

#include "ProblemInstance.h"

#include <list>
#include <memory>


namespace UVRP {

class AStarNode
{
public:
    ProblemInstance& pUVRP;

//    int x, y;
    Point pnt;
//    std::shared_ptr<AStarNode> parent;
    AStarNode * parent;
//    AStarNode * parent;
    double        f, g, h;

//    AStarNode() = default;
//    AStarNode(const ProblemInstance& pUVRP, std::shared_ptr<AStarNode> parent, int x, int y);
    AStarNode(ProblemInstance& pUVRP, AStarNode * parent, int x, int y);
    ~AStarNode() = default;


   std::vector<AStarNode *> adjacents(bool diagonal_movement);

   friend bool operator==(AStarNode&, AStarNode&);
};

bool operator==(AStarNode&, AStarNode&);

}

#endif // ASTARNODE_H
