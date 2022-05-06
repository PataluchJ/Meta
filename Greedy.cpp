#include "Greedy.h"
#include "TSPInstance.h"
#include <vector>
#include <algorithm>
#include <random>

Greedy::Greedy(){}

Solution Greedy::solve(InstancePointer instance)
{
    auto size = instance->getDistanceMatrix()->size();
    Solution currentSolution{};
    Solution bestSolution{};
    currentSolution.resize(size);
    bestSolution.resize(size);
    uint64_t bestSolutionCost = UINT64_MAX;
    for (uint32_t v = 0; v < size; v++) {
        solveFromStartingVerticle(instance, currentSolution, v);
        uint64_t cost = instance->calculateGenericSolutionDistance(currentSolution);
        if (cost < bestSolutionCost) {
            bestSolution.swap(currentSolution);
            bestSolutionCost = cost;
        }
    }
    return bestSolution;
}
void Greedy::solveFromStartingVerticle(InstancePointer instance, Solution& solutionVector, uint32_t startingVerticle)
{
    auto distanceMatrix = instance->getDistanceMatrix();
    auto size = distanceMatrix->size();
    
    std::vector<bool> visited;
    visited.resize(size, false);
    visited[startingVerticle] = true;

    uint32_t noVisited = 1;
    uint32_t currentVerticle = startingVerticle;
    uint32_t solutionIndex = 1;
    solutionVector[0] = startingVerticle;
    while (noVisited != size)
    {
        uint32_t bestVert = 0;
        uint64_t bestCost = UINT64_MAX;

        for (int j = 0; j < size; ++j)
        {
            //complexity++;
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

//        solutionVector->push_back(bestVert);
        solutionVector[solutionIndex] = bestVert;
        solutionIndex++;
        visited[bestVert] = true;
        ++noVisited;
        currentVerticle = bestVert;
    }
}

