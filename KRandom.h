#ifndef META_TSP_KRANDOM_H_
#define META_TSP_KRANDOM_H_

#include "Solver.h"
#include <vector>

class KRandom : public Solver
{
public:
    KRandom(uint32_t k);
    void solve(InstancePointer instance) override;
private:
    uint32_t k;
};

#endif
