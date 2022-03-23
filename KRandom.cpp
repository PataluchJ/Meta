#include "KRandom.h"
#include "TSPInstance.h"
#include <vector>
#include <algorithm>
#include <random>

KRandom::KRandom(u_int32_t k)
{
    this->k = k;
}

void KRandom::solve(InstancePointer instance)
{
    tempSolution = SolutionPointer(new Solution);

    for (int i = 0; i < instance->getDistanceMatrix()->size(); ++i)
        tempSolution->push_back(i);

    std::shuffle(tempSolution->begin(), tempSolution->end(), std::default_random_engine());
    instance->getSolution()->assign(tempSolution->begin(), tempSolution->end());
    minSolutionValue = instance->calculateSolutionDistance();

    for (int i = 0; i < k - 1; ++i)
    {
        std::shuffle(tempSolution->begin(), tempSolution->end(), std::default_random_engine());
        tempSolutionValue = instance->calculateGenericSolutionDistance(tempSolution);
        if (tempSolutionValue < minSolutionValue)
        {
            minSolutionValue = tempSolutionValue;
            instance->getSolution()->assign(tempSolution->begin(), tempSolution->end());
        }
    }
}
