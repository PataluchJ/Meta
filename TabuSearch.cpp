#include "TabuSearch.h"


TabuSearch::TabuSearch(SolverPointer initialSolver, Time ms, uint64_t tabuLenght)
    : initialSolver(initialSolver), endConditionValue(ms.get()), stopCondition(StopConditionType::Time), tabuLenght(tabuLenght)
{
    getConditionFunction = [this]() {
        return [this](RunStatistic& v) -> bool { 
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = now - v.start;
            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
            return time < endConditionValue; 
        };
    };
}

TabuSearch::TabuSearch(SolverPointer initialSolver, Iteratrions iters, uint64_t tabuLenght)
    : initialSolver(initialSolver), endConditionValue(iters.get()), stopCondition(StopConditionType::Iterations), tabuLenght(tabuLenght)
{
    getConditionFunction = [this]() {
        return [this](RunStatistic& v) -> bool {
            return v.iteration < endConditionValue;
        };
    };
}

TabuSearch::TabuSearch(SolverPointer initialSolver, NoImprovment iters, uint64_t tabuLenght)
    : initialSolver(initialSolver), endConditionValue(iters.get()), stopCondition(StopConditionType::Stagnation), tabuLenght(tabuLenght)
{
    getConditionFunction = [this]() {
        return [this](RunStatistic& v) -> bool {
            return v.stagnation < endConditionValue;
        };
    };
}

Solution TabuSearch::solve(InstancePointer instance)
{
    /* Init */
    HistoryList history;
    TabuList tabu(tabuLenght);

    /* Global scope solutions */
    Solution bestSolution = initialSolver->solve(instance);;
    Solution currentSolution = bestSolution;
    uint64_t bestSolutionCost = instance->calculateGenericSolutionDistance(bestSolution);

    /* Set statistic */
    RunStatistic rs;
    rs.start = std::chrono::high_resolution_clock::now();
    rs.stagnation = 0;
    rs.iteration = 0;

    /* Get condition function */
    auto conditionFunction = getConditionFunction();
    while (conditionFunction(rs)) {
       // std::cout << "I: " << rs.iteration << "\n";
        rs.iteration++;
        auto[neightboor, move, cost] = findBest(currentSolution, instance, tabu);
        tabu.add(move);
        currentSolution.swap(neightboor);

        /* Check if new solution is best since begining */
        if (cost < bestSolutionCost) {
            std::copy(currentSolution.begin(), currentSolution.end(), bestSolution.begin());
            bestSolutionCost = cost;
            rs.stagnation = 0;
        }
        else
            rs.stagnation++;

        /* Check if hisotry record should be saved */
        /// if (todo)
        //history.push_back(HistoryRecord(tabu, move, neightboor, cost));
    }

    return bestSolution;
}

TabuSearch::BNReturn TabuSearch::findBest(Solution& initial, InstancePointer instance, TabuList& tabu)
{
    Neighborhood n(initial);
    Solution neightboor;
    uint64_t bestCost = UINT64_MAX;
    Move bestMove;
    for (auto it = n.begin(), end = n.end(); it != end; it++) {
        if (tabu.contains(it.getCurrentMove())) {
            auto& vec = *it;
            auto cost = instance->calculateGenericSolutionDistance(vec);
            if (cost < bestCost) {
                bestCost = cost;
                neightboor.swap(vec);
                bestMove = it.getCurrentMove();
            }
        }
    }
    return std::make_tuple(neightboor, bestMove, bestCost);
}

void TabuSearch::TabuList::add(TabuSearch::Move& move)
{
    list[index] = move;
    index = (index + 1) % lenght;
}

bool TabuSearch::TabuList::contains(Move move)
{
    return (std::find(list.begin(), list.end(), move) == list.end());
}

TabuSearch::TabuList::TabuList(uint32_t lenght)
    : lenght(lenght), index(0)
{
    list.resize(lenght, Move(0, 0));
}
