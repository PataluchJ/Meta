#include "TabuSearch.h"


TabuSearch::TabuSearch(SolverPointer initialSolver, Time ms, uint64_t tabuLenght, uint64_t stagnationTreshold, uint64_t neighborhoodOffset)
    : initialSolver(initialSolver), endConditionValue(ms.get()), stopCondition(StopConditionType::Time),
        tabuLenght(tabuLenght), tabu(tabuLenght), neighborhoodOffset(neighborhoodOffset), stagnationTreshold(stagnationTreshold)
{
    conditionCheck = [this](RunStatistic& v) -> bool {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = now - v.start;
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        return time < endConditionValue;
    };
}

TabuSearch::TabuSearch(SolverPointer initialSolver, Iteratrions iters, uint64_t tabuLenght, uint64_t stagnationTreshold, uint64_t neighborhoodOffset)
    : initialSolver(initialSolver), endConditionValue(iters.get()), stopCondition(StopConditionType::Iterations), 
        tabuLenght(tabuLenght), tabu(tabuLenght), neighborhoodOffset(neighborhoodOffset), stagnationTreshold(stagnationTreshold)
{
    conditionCheck = [this](RunStatistic& v) -> bool {
        return v.iteration < endConditionValue;
    };
}

TabuSearch::TabuSearch(SolverPointer initialSolver, NoImprovment iters, uint64_t tabuLenght, uint64_t stagnationTreshold, uint64_t neighborhoodOffset)
    : initialSolver(initialSolver), endConditionValue(iters.get()), stopCondition(StopConditionType::Stagnation), 
        tabuLenght(tabuLenght), tabu(tabuLenght), neighborhoodOffset(neighborhoodOffset), stagnationTreshold(stagnationTreshold)
{
    conditionCheck = [this](RunStatistic& v) -> bool {
        return v.stagnation < endConditionValue;
    };
}

Solution TabuSearch::solve(InstancePointer instance)
{
    this->instance = instance;
    reset();

    while (conditionCheck(rs)) {
       // std::cout << "I: " << rs.iteration << "\n";
        rs.iteration++;
        rs.noStagIteration++;
        auto[neightboor, move, cost] = findBest();

        /* Check if history record should be saved */
        if (rs.iteration % (stagnationTreshold*2) == 0)
            history.push_back(HistoryRecord(tabu, move, currentSolution, rs.noStagIteration, cost));

        tabu.add(move);
        currentSolution.swap(neightboor);

        /* Check if new solution is better than current local */
        if (cost < bestLocalCost) {
            std::copy(currentSolution.begin(), currentSolution.end(), bestLocal.begin());
            bestLocalCost = cost;
            rs.stagnation = 0;
        }
        else
            rs.stagnation++;

        /* Stagnation detection */
        if (rs.stagnation > stagnationTreshold)
            stagnationRecovery();
    }

    return bestGlobal;
}

TabuSearch::BNReturn TabuSearch::findBest()
{
    Neighborhood n(currentSolution);
    
    auto size = n.end().getCurrentDistance();
    auto distancePerThread = size / noThreads;
    std::vector<Neighborhood::Iterator> iterators;
    iterators.push_back(n.begin());
    auto temp = n.begin();
    while (iterators.size() < noThreads) {
        temp.advance(distancePerThread);
        iterators.push_back(temp);
    }
    iterators.push_back(n.end());
    
    std::vector<std::future<BNReturn>> futures;
    for (auto i = 0; i < noThreads; i++) {
        futures.push_back(
            std::async(std::launch::async, [this, left = iterators[i], right = iterators[i+1]]() {return findBestInRange(left, right); })
        );
    }
    for (auto& future : futures) {
        future.wait();
    }
    auto [bestSolution, bestMove, bestCost] = futures[0].get();
    for (auto i = 1; i < futures.size(); i++) {
        auto [solution, move, cost] = futures[i].get();
        if (cost < bestCost) {
            bestSolution.swap(solution);
            bestMove = move;
            bestCost = cost;
        }
    }
    return BNReturn(bestSolution, bestMove, bestCost);
}


void TabuSearch::scramble(Solution& eggs)
{
    std::shuffle(eggs.begin(), eggs.end(), std::default_random_engine());
}

TabuSearch::BNReturn TabuSearch::findBestInRange(Neighborhood::Iterator left, Neighborhood::Iterator right)
{
    Solution neightboor;
    uint64_t bestCost = UINT64_MAX;
    Move bestMove;
    for (; left != right; left++) {
        auto& vec = *left;
        auto cost = instance->calculateGenericSolutionDistance(vec);
        if (!tabu.contains(left.getCurrentMove()) || (cost < bestGlobalCost && bestGlobalCost < bestLocalCost)) {
            if (cost < bestCost) {
                bestCost = cost;
                neightboor.swap(vec);
                bestMove = left.getCurrentMove();
            }
        }
    }
    /* Tabu Lock */
    /* TODO: Optimalize */
    /* Unsupported in multithread
    if (bestCost == UINT64_MAX) {
        auto m = Move(0, 0);
        tabu.add(m);
        return findBest();
    }
    */
    return std::make_tuple(neightboor, bestMove, bestCost);
}

void TabuSearch::stagnationRecovery()
{
    /* Compare local best to global */
    if (bestLocalCost < bestGlobalCost) {
        bestGlobal.swap(bestLocal);
        bestGlobalCost = bestLocalCost;
    }

    //std::cout << "Stagnation detected after " << rs.noStagIteration << " iterations. ";
    if (history.size() == 0) {
        //std::cout << "No point to revert to. Scrambling.\n";
        scramble(currentSolution);

    }
    else {
        auto& rec = history.back();
        currentSolution.assign(rec.solution.begin(), rec.solution.end());
        tabu = rec.tabu;
        tabu.add(rec.lastMove);
        history.pop_back();
        rs.stagnation = 0;
        rs.noStagIteration = rs.iteration;
        //std::cout << "Reverted to iteration: " << rec.iteration << "\n";
    }
}

void TabuSearch::reset()
{
    /* Memory */
    history.clear();
    tabu = TabuList(tabuLenght);

    /* Global */
    bestGlobal = initialSolver->solve(instance);
    bestGlobalCost = instance->calculateGenericSolutionDistance(bestGlobal);

    /* Local */
    bestLocal = bestGlobal;
    bestLocalCost = bestGlobalCost;

    currentSolution = bestGlobal;

    /* Set statistic */
    rs.start = std::chrono::high_resolution_clock::now();
    rs.stagnation = 0;
    rs.iteration = 0;
    rs.noStagIteration = 0;
}

void TabuSearch::TabuList::add(TabuSearch::Move& move)
{
    list[index] = move;
    index = (index + 1) % lenght;
}

bool TabuSearch::TabuList::contains(Move move)
{
    return (std::find(list.begin(), list.end(), move) != list.end());
}

TabuSearch::TabuList::TabuList(uint32_t lenght)
    : lenght(lenght), index(0)
{
    list.resize(lenght, Move(0, 0));
}
