#ifndef META_TSP_KRANDOM_H_
#define META_TSP_KRANDOM_H_

#include "Solver.h"
#include <vector>

class KRandom : public Solver
{
public:
    KRandom(u_int32_t k);
    void solve(InstancePointer instance);

private:
    u_int32_t k;
    uint32_t minSolutionValue;
    uint32_t tempSolutionValue;
    SolutionPointer tempSolution;
};

#endif
