#ifndef META_TSP_GREEDY_H_
#define META_TSP_GREEDY_H_

#include "Solver.h"
#include <vector>

class Greedy : public Solver
{
public:
    Greedy(uint32_t startingVerticle);
    void solve(InstancePointer instance) override;
private:
    uint32_t startingVerticle;
};

#endif
