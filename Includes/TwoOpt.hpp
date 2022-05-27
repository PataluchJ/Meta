#ifndef META_TSP_TWOOPT_H_
#define META_TSP_TWOOPT_H_

#include <Solver.hpp>

class TwoOpt : public Solver
{
public:
	TwoOpt(SolverPointer initialSolver) : initialSolver(initialSolver) {}
	~TwoOpt() {}

	Solution solve(InstancePointer instance) override;
	
	std::string getName() override { return "2Opt"; }
private:
	SolverPointer initialSolver;
};

class TwoOptOptymalizer : public InPlaceOptymalizer
{
public:
	TwoOptOptymalizer(InstancePointer context) : context(context) {}

	uint64_t optimize(Solution& solution) override;

private:
	InstancePointer context;
};
#endif

