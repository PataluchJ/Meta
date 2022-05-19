#ifndef META_TSP_GREEDY_H_
#define META_TSP_GREEDY_H_

#include <Solver.hpp>

class Greedy : public Solver
{
public:
    Greedy();
    Solution solve(InstancePointer instance) override;
    std::string getName() override { return "Greedy"; }
private:
    void solveFromStartingVerticle(InstancePointer instance, Solution& solutin, uint32_t startingVerticle);
};

#endif
