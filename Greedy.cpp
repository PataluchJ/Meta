#include "Greedy.h"
#include "TSPInstance.h"
#include <vector>
#include <algorithm>
#include <random>

Greedy::Greedy(u_int32_t starting_verticle)
{
    this->startingVerticle = starting_verticle;
}

void Greedy::solve(InstancePointer instance)
{
    size = instance->getDistanceMatrix()->size();
    visited = new bool[size];
    for (int i = 0; i < size; ++i)
        visited[i] = false;

    instance->getSolution()->push_back(startingVerticle);
    visited[startingVerticle] = true;
    noVisited = 1;
    currentVerticle = startingVerticle;

    while (noVisited != size)
    {
        bestVert = 0;
        bestCost = UINT32_MAX;

        for (int j = 0; j < size; ++j)
        {
            if (!visited[j] && j != currentVerticle)
            {
                cost = instance->getDistanceMatrix()->at(currentVerticle).at(j);
                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestVert = j;
                }
            }
        }

        instance->getSolution()->push_back(bestVert);
        visited[bestVert] = true;
        ++noVisited;
        currentVerticle = bestVert;
    }

    delete[] visited;
};
