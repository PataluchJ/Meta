#ifndef META_TABUSEARCH_H_
#define META_TABUSEARCH_H_

#include "Solver.h"
#include "NamedType.h"
#include <functional>
#include <tuple>
#include <chrono>
#include <list>
#include <random>
#include <thread>
#include <future>

class TabuSearch : public Solver
{
	struct TimeParam {};
	struct IterationsParam {};
	struct NoImprovmentParam {};
public:
	using Time = NamedType<uint64_t, TimeParam>;
	using Iteratrions = NamedType<uint64_t, IterationsParam>;
	using NoImprovment = NamedType<uint64_t, NoImprovmentParam>;

	explicit TabuSearch(SolverPointer initialSolver, Time ms, uint64_t tabuLenght = 7, uint64_t stagnationThreshold = 200, uint64_t neighborhoodOffset = 0);
	explicit TabuSearch(SolverPointer initialSolver, Iteratrions iters, uint64_t tabuLenght = 7, uint64_t stagnationThreshold = 200, uint64_t neighborhoodOffset = 0);
	explicit TabuSearch(SolverPointer initialSolver, NoImprovment iters, uint64_t tabuLenght = 7, uint64_t stagnationThreshold = 200, uint64_t neighborhoodOffset = 0);
	Solution solve(InstancePointer instance) override;
	std::string getName() override { return "TabuSearch"; };

private:
	/* Structs forward declaration */
	struct RunStatistic;
	struct TabuList;
	struct HistoryRecord;

	/* Types */
	using Move = Neighborhood::Iterator::Move;
	using BNReturn = std::tuple<Solution, Move, uint64_t>;
	using ConditionFunction = std::function<bool(RunStatistic&)>;
	using HistoryList = std::list<HistoryRecord>;
	using BNPackage = std::packaged_task<BNReturn(Neighborhood::Iterator, Neighborhood::Iterator)>;

	/* Enums */
	enum class StopConditionType {
		Time,
		Iterations,
		Stagnation
	};

	/* Data structs */
	struct TabuList {
		void add(Move& move);
		bool contains(Move move);
		TabuList(uint32_t lenght);
	private:
		uint32_t lenght;
		uint32_t index;
		std::vector<Move> list;
	};

	struct RunStatistic {
		std::chrono::steady_clock::time_point start;
		uint64_t iteration;
		uint64_t stagnation;
		uint64_t noStagIteration;
	};

	struct HistoryRecord {
		TabuList tabu;
		Move lastMove;
		Solution solution;
		uint64_t iteration;
		uint64_t cost;
		HistoryRecord(TabuList& tabu, Move& move, Solution& solution, uint64_t iteration, uint64_t cost)
			: tabu(tabu), lastMove(move), solution(solution), iteration(iteration), cost(cost) {}
	};

	/* Config variables */
	StopConditionType stopCondition;
	uint32_t historyLenght;
	uint32_t tabuLenght;
	uint64_t endConditionValue;
	uint64_t stagnationTreshold;
	uint64_t neighborhoodOffset;
	uint8_t noThreads = 2;

	/* Run variables */
	SolverPointer initialSolver;
	ConditionFunction conditionCheck;
	InstancePointer instance;
	Solution bestGlobal, bestLocal, currentSolution;
	uint64_t bestGlobalCost, bestLocalCost;

	HistoryList history;
	TabuList tabu;
	RunStatistic rs;

	BNReturn findBest();
	BNReturn findBestInRange(Neighborhood::Iterator left, Neighborhood::Iterator right);
	void scramble(Solution& eggs);
	void stagnationRecovery();
	void reset();
};

#endif