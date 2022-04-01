#include "TwoOpt.h"

void TwoOpt::solve(InstancePointer instance)
{
	complexity = 0;
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

		/* Generate and check all neightboors */
		for (auto left = neightboor->begin(); left != neightboor->end(); left++) {
			for (auto right = left + 1; right != neightboor->end(); right++) {
				//complexity++;
				//std::swap((*neightboor)[v], (*neightboor)[u]);
				std::reverse(left, right);
				auto currentCost = instance->calculateGenericSolutionDistance(neightboor);
				if (currentCost < bestNeightboorCost) {
					bestNeightboorCost = currentCost;
					std::copy(neightboor->begin(), neightboor->end(), bestNeightboor->begin());
				}
				/* Reswap */
				//std::swap((*neightboor)[v], (*neightboor)[u]);
				std::reverse(left, right);
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
