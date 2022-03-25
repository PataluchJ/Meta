#ifndef META_TSP_SOLVER_H_
#define META_TSP_SOLVER_H_

#include "TSPInstance.h"

class Solver
{
public:
	virtual void solve(InstancePointer instance) = 0;
};

#endif