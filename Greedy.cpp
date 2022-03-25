#include "Greedy.h"
#include "TSPInstance.h"
#include <vector>
#include <algorithm>
#include <random>

Greedy::Greedy(uint32_t startingVerticle)
    : startingVerticle(startingVerticle)
{}

void Greedy::solve(InstancePointer instance)
{
    auto size = instance->getDistanceMatrix()->size();
    auto distanceMatrix = instance->getDistanceMatrix();
    std::vector<bool> visited;
    visited.resize(size, false);

    auto solutionVector = instance->getSolution();
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
};
