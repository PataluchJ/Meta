#ifndef META_TSP_GREEDY_H_
#define META_TSP_GREEDY_H_

#include "Solver.h"
#include <vector>

class Greedy : public Solver
{
public:
    Greedy();
    void solve(InstancePointer instance) override;
private:
    void solveFromStartingVerticle(InstancePointer instance, SolutionPointer solutin, uint32_t startingVerticle);
};

#endif
