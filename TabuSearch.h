#ifndef META_TABUSEARCH_H_
#define META_TABUSEARCH_H_

#include "Solver.h"
#include "NamedType.h"
#include <functional>
#include <tuple>
#include <chrono>
#include <list>

class TabuSearch : public Solver
{
	struct TimeParam {};
	struct IterationsParam {};
	struct NoImprovmentParam {};
public:
	using Time = NamedType<uint64_t, TimeParam>;
	using Iteratrions = NamedType<uint64_t, IterationsParam>;
	using NoImprovment = NamedType<uint64_t, NoImprovmentParam>;

	explicit TabuSearch(SolverPointer initialSolver, Time ms, uint64_t tabuLenght);
	explicit TabuSearch(SolverPointer initialSolver, Iteratrions iters, uint64_t tabuLenght);
	explicit TabuSearch(SolverPointer initialSolver, NoImprovment iters, uint64_t tabuLenght);
	Solution solve(InstancePointer instance) override;
	std::string getName() override { return "TabuSearch"; };

private:
	struct RunStatistic {
		std::chrono::steady_clock::time_point start;
		uint64_t iteration;
		uint64_t stagnation;
	};

	using Move = Neighborhood::Iterator::Move;
	using BNReturn = std::tuple<Solution, Move, uint64_t>;
	using ConditionFunction = std::function<bool(RunStatistic&)>;
	using ConditionFactory = std::function<ConditionFunction()>;

	ConditionFactory getConditionFunction;

	SolverPointer initialSolver;

	std::function<std::function<bool(uint64_t)>()> conditionFactory;
	std::function<bool()> conditionCheck;

	enum class StopConditionType {
		Time,
		Iterations,
		Stagnation
	} stopCondition;
	uint64_t endConditionValue;
	uint32_t tabuLenght;

	struct BestNeightboor {
		Solution neightboor;
		Move move;
		uint64_t cost;
	};

	struct TabuList {
		void add(Move& move);
		bool contains(Move move);
		TabuList(uint32_t lenght);
	private:
		uint32_t lenght;
		uint32_t index;
		std::vector<Move> list;
	};
	
	uint32_t historyLenght;
	struct HistoryRecord {
		TabuList tabu;
		Move lastMove;
		Solution solution;
		uint64_t solutionCost;
		HistoryRecord(TabuList& tabu, Move& move, Solution& solution, uint64_t cost)
			: tabu(tabu), lastMove(move), solution(solution), solutionCost(cost) {}
	};
	using HistoryList = std::list<HistoryRecord>;

	BNReturn findBest(Solution& initial, InstancePointer instance, TabuList& tabu);
};

#endif