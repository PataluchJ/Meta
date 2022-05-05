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

	enum class TabuLenght {
		Root,
		Normal,
		Square
	};

	explicit TabuSearch(SolverPointer initialSolver, Time ms, TabuLenght tl, uint64_t stagnationThreshold = 200, uint64_t neighborhoodOffset = 0);
	explicit TabuSearch(SolverPointer initialSolver, Iteratrions iters, TabuLenght tl, uint64_t stagnationThreshold = 200, uint64_t neighborhoodOffset = 0);
	explicit TabuSearch(SolverPointer initialSolver, NoImprovment iters, TabuLenght tl, uint64_t stagnationThreshold = 200, uint64_t neighborhoodOffset = 0);
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
	
	struct TabuMatrix {
		TabuMatrix(uint32_t size, uint32_t lenght);
		void add(Move move);
		bool avaible(Move move);
	private:
		using Matrix = std::vector<std::vector<uint64_t>>;
		Matrix matrix;
		uint32_t lenght;
		uint32_t size;
		uint64_t iteration;
	};

	struct RunStatistic {
		std::chrono::steady_clock::time_point start;
		uint64_t iteration;
		uint64_t stagnation;
		uint64_t noStagIteration;
	};

	struct HistoryRecord {
		TabuMatrix tabu;
		Move lastMove;
		Solution solution;
		uint64_t iteration;
		uint64_t cost;
		HistoryRecord(TabuMatrix& tabu, Move& move, Solution& solution, uint64_t iteration, uint64_t cost)
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
	TabuLenght tl;

	/* Run variables */
	SolverPointer initialSolver;
	ConditionFunction conditionCheck;
	InstancePointer instance;
	Solution bestGlobal, bestLocal, currentSolution;
	uint64_t bestGlobalCost, bestLocalCost;

	HistoryList history;
	TabuMatrix tabu;
	RunStatistic rs;

	BNReturn findBest();
	//static BNReturn findBestInRange(Neighborhood::Iterator left, Neighborhood::Iterator right, InstancePointer instance);
	void scramble(Solution& eggs);
	void stagnationRecovery();
	void reset();
};

#endif