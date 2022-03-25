#include "TwoOpt.h"

void TwoOpt::solve(InstancePointer instance)
{
	auto matrix = instance->getDistanceMatrix();
	auto size = matrix->size();
	
	/* Get starting solution from greedy */
	Greedy g;
	g.solve(instance);

	//SolutionPointer bestSolution = SolutionPointer(new Solution);
	SolutionPointer bestSolution = instance->getSolution();
	SolutionPointer bestNeightboor = SolutionPointer(new Solution);
	SolutionPointer neightboor = SolutionPointer(new Solution);

	//bestSolution->resize(size, 0);
	bestNeightboor->resize(size, 0);
	neightboor->resize(size, 0);

	uint64_t bestSolutionCost = instance->calculateSolutionDistance();
	//uint64_t bestNeightboorCost = UINT64_MAX;
	uint64_t currentNeightboorCost = UINT64_MAX;

	bool improved = true;
	do {
		uint64_t bestNeightboorCost = bestSolutionCost;
		std::copy(bestSolution->begin(), bestSolution->end(), neightboor->begin());
		std::copy(bestSolution->begin(), bestSolution->end(), bestNeightboor->begin());

		/* Generate and check all permutations */
		for (uint32_t v = 0; v < size; v++) {
			for (uint32_t u = v+1; u < size; u++) {
				std::swap((*neightboor)[v], (*neightboor)[u]);
				auto currentCost = instance->calculateGenericSolutionDistance(neightboor);
				if (currentCost < bestNeightboorCost) {
					bestNeightboorCost = currentCost;
					std::copy(neightboor->begin(), neightboor->end(), bestNeightboor->begin());
				}
				/* Reswap */
				std::swap((*neightboor)[v], (*neightboor)[u]);
			}
		}
		/* Check if best neightboor is better than current solution */
		if (bestNeightboorCost < bestSolutionCost) {
			bestSolutionCost = bestNeightboorCost;
			std::copy(bestNeightboor->begin(), bestNeightboor->end(), bestSolution->begin());
		}
		else
			improved = false;

	} while (improved);
}
