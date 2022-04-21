#ifndef META_TSP_SOLVER_H_
#define META_TSP_SOLVER_H_

#include "TSPInstance.h"

class Solver
{
public:
	virtual Solution solve(InstancePointer instance) = 0;
	virtual std::string getName() = 0;
protected:
};

#endif