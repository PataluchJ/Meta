#ifndef META_TSP_SOLVER_H_
#define META_TSP_SOLVER_H_

#include <TSPInstance.hpp>

class Solver
{
public:
	virtual Solution solve(InstancePointer instance) = 0;
	virtual std::string getName() = 0;
protected:
};

using SolverPointer = std::shared_ptr<Solver>;

class InPlaceOptymalizer {
public:
	uint64_t optimize(Solution& solution) = 0;
private:
	InstancePointer context;
};

using OptymalizerPointer = std::shared_ptr<InPlaceOptymalizer>;

#endif