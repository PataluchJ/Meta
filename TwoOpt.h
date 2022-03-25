#ifndef META_TSP_TWOOPT_H_
#define META_TSP_TWOOPT_H_
#include "Solver.h"
#include "Greedy.h"
#include <vector>

class TwoOpt : public Solver
{
public:
	TwoOpt() {}
	~TwoOpt() {}

	void solve(InstancePointer instance) override;
};
#endif

