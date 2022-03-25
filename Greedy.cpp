#include "Greedy.h"
#include "TSPInstance.h"
#include <vector>
#include <algorithm>
#include <random>

Greedy::Greedy(){}

void Greedy::solve(InstancePointer instance)
{
    SolutionPointer solution = SolutionPointer(new Solution);
    SolutionPointer bestSolution = instance->getSolution();;
    uint64_t bestSolutionCost = UINT64_MAX;
    auto matrix = instance->getDistanceMatrix();
    for (uint32_t v = 0; v < matrix->size(); v++) {
        solveFromStartingVerticle(instance, solution, v);
        uint64_t cost = instance->calculateGenericSolutionDistance(solution);
        if (cost < bestSolutionCost) {
            std::copy(solution->begin(), solution->end(), bestSolution->begin());
        }
    }
}
void Greedy::solveFromStartingVerticle(InstancePointer instance, SolutionPointer solutionVector, uint32_t startingVerticle)
{
    auto distanceMatrix = instance->getDistanceMatrix();
    auto size = distanceMatrix->size();
    
    std::vector<bool> visited;
    visited.resize(size, false);

    solutionVector->clear();
    solutionVector->reserve(size);
    solutionVector->push_back(startingVerticle);
    visited[startingVerticle] = true;

    uint32_t noVisited = 1;
    uint32_t currentVerticle = startingVerticle;

    while (noVisited != size)
    {
        uint32_t bestVert = 0;
        uint64_t bestCost = UINT64_MAX;

        for (int j = 0; j < size; ++j)
        {
            if (!visited[j] && j != currentVerticle)
            {
                uint64_t cost = (*distanceMatrix)[currentVerticle][j];
                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestVert = j;
                }
            }
        }

        solutionVector->push_back(bestVert);
        visited[bestVert] = true;
        ++noVisited;
        currentVerticle = bestVert;
    }
}

