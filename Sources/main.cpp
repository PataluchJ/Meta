#include <TwoOpt.hpp>
#include <TSPInstance.hpp>
#include <iostream>
#include <Genetic.hpp>

int main(int argc, char** argv) {
	auto instance = TSPInstance::loadFromFile("TSPData/gr120.tsp");
	srand(time(NULL));

	GA::Genetic::Parameters par;
	par.kidsPopulation = 1000;
	par.parentsPopulation = 1000;
	par.crossover = std::make_shared<GA::PMXCrossover>(GA::PMXCrossover(instance));
	par.includeParents = false;
//	OptymalizerPointer optimized = std::make_shared< TwoOptOptymalizer>(TwoOptOptymalizer(instance));
//	par.crossover = std::make_shared<GA::PMXLocalOptCrossover>(GA::PMXLocalOptCrossover(optimized));
	par.parentSelector = std::make_shared<GA::RouletteParentSelector>(GA::RouletteParentSelector());
	par.selection = std::make_shared<GA::RouletteSelector>(GA::RouletteSelector());
//	par.selection = std::make_shared<GA::RandomSelector>(GA::RandomSelector());
	par.elitism = 0.5;
	par.timeLimit = 60000;
	par.mutation = 0.05;
	GA::Genetic gen(par);
	
	auto solution = gen.solve(instance);
	std::cout << "Solved: " << instance->calculateGenericSolutionDistance(solution) << "\n";
	return 0;
}