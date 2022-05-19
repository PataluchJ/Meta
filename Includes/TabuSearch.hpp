#ifndef META_TABUSEARCH_H_
#define META_TABUSEARCH_H_

#include <Solver.hpp>
#include <NamedType.hpp>

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

	struct TabuSearchConfig {
		TabuLenght tabuLength;
		uint32_t historyLength;
		uint64_t stagnationTreshold;
		uint64_t neightborhoodStep;
		Neighborhood::NeighborhoodFunction neighborhoodType;

		TabuSearchConfig() 
			: tabuLength(TabuLenght::Root), historyLength(50), stagnationTreshold(1000), neightborhoodStep(1),
			neighborhoodType(Neighborhood::NeighborhoodFunction::Reverse) {}
	};

	explicit TabuSearch(SolverPointer initialSolver, TabuSearchConfig config);
	explicit TabuSearch(SolverPointer initialSolver, Time ms, TabuSearchConfig config);
	explicit TabuSearch(SolverPointer initialSolver, Iteratrions, TabuSearchConfig config);
	explicit TabuSearch(SolverPointer initialSolver, NoImprovment, TabuSearchConfig config);
	Solution solve(InstancePointer instance) override;
	std::string getName() override { return "TabuSearch"; };

	void setPRDReference(uint64_t value);

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
		std::chrono::high_resolution_clock::time_point start;
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
	Neighborhood::NeighborhoodFunction neighborhoodFunction;

	/* Run variables */
	SolverPointer initialSolver;
	ConditionFunction conditionCheck;
	InstancePointer instance;
	Solution bestGlobal, bestLocal, currentSolution, initialSolution;
	uint64_t bestGlobalCost, bestLocalCost;

	HistoryList history;
	TabuMatrix tabu;
	RunStatistic rs;

	uint64_t PRDReference;

	BNReturn findBest();
	//static BNReturn findBestInRange(Neighborhood::Iterator left, Neighborhood::Iterator right, InstancePointer instance);
	void scramble();
	void stagnationRecovery();
	void reset();
};

#endif