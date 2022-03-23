#ifndef META_TSP_GREEDY_H_
#define META_TSP_GREEDY_H_

#include "Solver.h"
#include <vector>

class Greedy : public Solver
{
public:
    Greedy(u_int32_t starting_verticle);
    void solve(InstancePointer instance);

private:
    bool *visited;
    u_int32_t startingVerticle;
    u_int32_t currentVerticle;
    u_int32_t noVisited;
    u_int32_t size;
    u_int32_t bestVert;
    u_int32_t bestCost;
    u_int32_t cost;
};

#endif
