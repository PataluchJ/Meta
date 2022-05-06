#ifndef META_TSP_TWOOPT_H_
#define META_TSP_TWOOPT_H_

#include "Solver.h"
#include "Greedy.h"

class TwoOpt : public Solver
{
public:
	TwoOpt() {}
	~TwoOpt() {}

	Solution solve(InstancePointer instance) override;
	std::string getName() override { return "2Opt"; }
};
#endif

