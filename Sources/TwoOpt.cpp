#include <TwoOpt.hpp>

Solution TwoOpt::solve(InstancePointer instance)
{
	auto matrix = instance->getDistanceMatrix();
	auto size = matrix->size();
	
	Solution bestSolution = initialSolver->solve(instance);
	uint64_t bestSolutionCost = instance->calculateGenericSolutionDistance(bestSolution);

	bool improved = true;
	do {
		improved = false;
		Neighborhood n(bestSolution);
		for (auto& neightboor : n) {
			auto currentCost = instance->calculateGenericSolutionDistance(neightboor);
			if (currentCost < bestSolutionCost) {
				bestSolution.swap(neightboor);
				bestSolutionCost = currentCost;
				improved = true;
			}
		}
	} while (improved);

	return bestSolution;
}