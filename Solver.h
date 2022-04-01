#ifndef META_TSP_SOLVER_H_
#define META_TSP_SOLVER_H_

#include "TSPInstance.h"

class Solver
{
public:
	Solver() : complexity(0) {}
	virtual void solve(InstancePointer instance) = 0;
	virtual std::string getName() = 0;
	virtual size_t getComplexity() { return complexity; }
protected:
	size_t complexity;
};

#endif