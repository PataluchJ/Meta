#ifndef META_GENETIC_H_
#define META_GENETIC_H_

#include <Solver.hpp>
#include <Logger.hpp>
#include <functional>
#include <chrono>
/*
* TODO: 
* [X] Getting initial population
* [X] Selection (random, roulet)
* [~] Crossover (multiple)
* [] Choosing next generation (if parents can survive)
* [] Multithreading and islands
* [] Mutations
* [] Params. modyfication by algo.
* [] Islands population crossover
* 
*/
namespace Gen {

	/* Data types */
	struct Specimen {
		Solution solution;
		uint64_t cost;
	};

	using Population = std::vector<Specimen>;
	using PopulationPointer = std::shared_ptr<Population>;
	struct Parents {
		Specimen* a;
		Specimen* b;
	};
	using SelectedParents = std::vector<Parents>;


	/* Functions for selecting parents pairs */
	class Mating {
	public:
		using Function = std::function<SelectedParents && (PopulationPointer population, uint32_t numberOfPairs)>;
		//using FunctionGenerator = std::function<Function()>;
		static Function getRandom();
		static Function getRoulette();
	};
	
	/* Crossover functions */
	class Crossover {
	public:
		using Function = std::function<void(const Specimen* parentA, const Specimen* parent2B, Specimen* kidA, Specimen* kidB, float mutation)>;

		static Function getPMX();
		static Function getPMXWithLocalOpt(OptymalizerPointer optimizer);
	private:
		static void pmxSwap(const Specimen* sourc, Specimen* targ, uint8_t pivot);
	};

	/* Functions for selection of speciments into next generation
	*	and calcualtes current costs for surviving population
	*/
	class Selection {
	public: 
		using Function = std::function<void(PopulationPointer parents, PopulationPointer kids, uint32_t desiredPopulation, float selectionElitism)>;

		static Function getRoulette();
	private:
		struct CombinePointers {
			CombinePointers(PopulationPointer a, PopulationPointer b);
			Specimen& operator[](size_t index);
		private:
			size_t bStart;
			PopulationPointer a, b;
		};
	};


	class Genetic : public Solver
	{
	public:

		struct Parameters {
			/* Population size control*/
			uint32_t maxPopulation;
			uint32_t minPopulation;
			uint32_t desiredPopulation;
			float elitism;
			
			/* Mutation, will increase with stagnation */
			float minimumMutation;
			float maximumMutation;
			float currentMutation;

			/* Stagnation detection and recovery */
			uint32_t mutationIncreaseAt;
			uint32_t mutationMaxAt;
			uint32_t nukeAt;

			/* Population change functions */
			Selection::Function selection;
			Crossover::Function crossover;
			Mating::Function mate;


			Solution initialSolution;

		};

		struct RunStatistic {
			std::chrono::high_resolution_clock::time_point start;
			uint64_t iteration;
			uint64_t stagnation;
		};

		Genetic(Parameters& p) : params(p) {}
		~Genetic() {}

		Solution solve(InstancePointer instance) override;
		std::string getName() override { return std::string("Genetic"); };
	private:
		Parameters params;

		void generateRandomPopulation(InstancePointer instance, PopulationPointer population, uint32_t size);
	};
}
#endif
