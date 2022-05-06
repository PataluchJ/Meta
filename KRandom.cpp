#include "KRandom.h"
#include "TSPInstance.h"
#include <vector>
#include <algorithm>
#include <random>

KRandom::KRandom(uint32_t k) : k(k) {}

Solution KRandom::solve(InstancePointer instance)
{
    auto matrix = instance->getDistanceMatrix();
    Solution tempSolution{};
    Solution bestSolution{};
    tempSolution.resize(matrix->size());
    bestSolution.resize(matrix->size());
    for (uint32_t i = 0; i < matrix->size(); i++) {
        bestSolution[i] = i;
        tempSolution[i] = i;
    }

    uint64_t bestCost = UINT64_MAX;

    for (int i = 0; i < k; ++i)
    {
        std::shuffle(tempSolution.begin(), tempSolution.end(), std::default_random_engine());
        uint64_t cost = instance->calculateGenericSolutionDistance(tempSolution);
        if (bestCost > cost)
        {
            bestCost = cost;
            bestSolution.swap(tempSolution);
        }
    }

    return bestSolution;
}
