#include "TabuSearch.h"


TabuSearch::TabuSearch(SolverPointer initialSolver, Time ms, TabuLenght tl, uint64_t stagnationTreshold, uint64_t neighborhoodOffset)
    : initialSolver(initialSolver), endConditionValue(ms.get()), stopCondition(StopConditionType::Time),
        tabuLenght(tabuLenght), tabu(0,0), tl(tl), neighborhoodOffset(neighborhoodOffset), stagnationTreshold(stagnationTreshold)
{
    conditionCheck = [this](RunStatistic& v) -> bool {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = now - v.start;
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        return time < endConditionValue;
    };
}

TabuSearch::TabuSearch(SolverPointer initialSolver, Iteratrions iters, TabuLenght tl, uint64_t stagnationTreshold, uint64_t neighborhoodOffset)
    : initialSolver(initialSolver), endConditionValue(iters.get()), stopCondition(StopConditionType::Iterations), 
        tabuLenght(tabuLenght), tabu(0,0), tl(tl), neighborhoodOffset(neighborhoodOffset), stagnationTreshold(stagnationTreshold)
{
    conditionCheck = [this](RunStatistic& v) -> bool {
        return v.iteration < endConditionValue;
    };
}

TabuSearch::TabuSearch(SolverPointer initialSolver, NoImprovment iters, TabuLenght tl, uint64_t stagnationTreshold, uint64_t neighborhoodOffset)
    : initialSolver(initialSolver), endConditionValue(iters.get()), stopCondition(StopConditionType::Stagnation), 
        tabuLenght(tabuLenght), tabu(0,0), tl(tl), neighborhoodOffset(neighborhoodOffset), stagnationTreshold(stagnationTreshold)
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
    Solution neightboor;
    uint64_t bestCost = UINT64_MAX;
    Move bestMove;
    for (auto it = n.begin(), end = n.end(); it != end; it++) {
       // std::cout << it.getCurrentDistance() << ": " << it.getCurrentMove().l << " " << it.getCurrentMove().r << "\n";
        if (tabu.avaible(it.getCurrentMove())) {
            auto& vec = *it;
            auto cost = instance->calculateGenericSolutionDistance(vec);
            if (cost < bestCost) {
                bestCost = cost;
                neightboor.swap(vec);
                bestMove = it.getCurrentMove();
            }
        }
    }

    return BNReturn(neightboor, bestMove, bestCost);
}


void TabuSearch::scramble(Solution& eggs)
{
    std::shuffle(eggs.begin(), eggs.end(), std::default_random_engine());
}

/*
TabuSearch::BNReturn TabuSearch::findBestInRange(Neighborhood::Iterator left, Neighborhood::Iterator right, InstancePointer instance)
{
    Solution neightboor;
    uint64_t bestCost = UINT64_MAX;
    Move bestMove;
    for (; left != right; left++) {
        auto& vec = *left;
        auto cost = instance->calculateGenericSolutionDistance(vec);
        if (!tabu.avaible(left.getCurrentMove()) || (cost < bestGlobalCost && bestGlobalCost < bestLocalCost)) {
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
    *//*
    return std::make_tuple(neightboor, bestMove, bestCost);
}*/

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
    auto size = instance->getDistanceMatrix()->size();
    switch (tl) {
    case TabuLenght::Root: {
        tabu = TabuMatrix(size, sqrt(size));
    }break;
    case TabuLenght::Normal: {
        tabu = TabuMatrix(size, size);
    }break;
    case TabuLenght::Square: {
        tabu = TabuMatrix(size, size*size);
    }break;
    }
    

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

TabuSearch::TabuMatrix::TabuMatrix(uint32_t size, uint32_t lenght)
    : size(size), lenght(lenght), iteration(0)
{
    matrix.resize(size);
    for(auto& v : matrix)
        v.resize(size, 0);
}

void TabuSearch::TabuMatrix::add(Move move)
{
    matrix[move.l][move.r] = iteration + lenght + 1;
    iteration++;
}

bool TabuSearch::TabuMatrix::avaible(Move move)
{
    return (matrix[move.l][move.r] >= iteration);
}
