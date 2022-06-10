#include <TwoOpt.hpp>
#include <TSPInstance.hpp>
#include <iostream>
#include <Genetic.hpp>
#include <Greedy.hpp>

using namespace GA;

void runTests(std::vector<Solver*>& solvers, std::vector<std::string>& solversNames, std::string filepath) {
	std::fstream output(filepath, std::ios_base::out);
	std::string path = "TSPData/";
	std::vector<std::string> tspinstances{
		"a280", "att48", "bayg29", "berlin52", 
		"brg180", "ch130", "ch150", "eil101", "eil51", 
		"eil76", "fri26", "gr120", "gr202", "gr24", "gr48", 
		"gr666", "gr96", "kroA100", "kroC100", "kroD100", 
		"lin105", "pa561", "pcb442", "pr1002", "pr2392", 
		"pr76", "rd100", "st70", "tsp225", "ulysses16", 
		"ulysses22"
	};

	std::cout << "Instance";
	output << "Instance";
	for (auto solverName : solversNames) {
		std::cout << ";" << solverName;
		output << ";" << solverName;
	}
	output << "\n";
	std::cout << "\n";


	for (auto name : tspinstances) {
		auto instance = TSPInstance::loadFromFile(path + name + ".tsp");
		auto tour = TSPInstance::loadTourFromFile(path + name + ".opt.tour");
		if (instance->getDistanceMatrix()->size() == 0 || tour.size() == 0) {
			Logger::logNotCriticalError("Unable to open instance " + name);
			continue;
		}
		auto reference = instance->calculateGenericSolutionDistance(tour);
		instance->setReferenceValue(reference);

		output << name;
		std::cout << name;
		for (uint32_t i = 0; i < solvers.size(); i++) {
			auto result = solvers[i]->solve(instance);
			auto value = instance->calculateGenericSolutionDistance(result);
			double prd = (double)(value - reference) / (double)(reference);
			std::cout << ";" << prd;
			output << ";" << prd;
		}
		std::cout << "\n";
		output << "\n";
		output.flush();
	}
}

void runRandomTests(std::vector<Solver*>& solvers, std::vector<std::string>& solversNames, std::string filepath, uint32_t sizeMin=20, uint32_t sizeMax=220, uint32_t sizeStep=50, uint32_t testsPerSize = 5) {
	int seed = 0;

	std::fstream output(filepath, std::ios_base::out);
	output << "Size"; std::cout << "Size";
	for (auto name : solversNames) {
		output << ";" << name; std::cout << ";" << name;
	}
	output << "\n"; std::cout << "\n";
	for (uint32_t size = sizeMin; size <= sizeMax; size += sizeStep) {
		for (uint32_t test = 0; test < testsPerSize; test++) {
			auto instance = TSPInstance::generateSymtericInstaance(size, seed++);
			std::cout << size; output << size;
			for (int i = 0; i < solvers.size(); i++) {
				auto solution = solvers[i]->solve(instance);
				auto result = instance->calculateGenericSolutionDistance(solution);
				output << ";" << result; std::cout << ";" << result;
			}
			std::cout << "\n"; output << "\n";
		}
	}
}

void beginTestForMutation() {
	auto roulPS = std::make_shared<RouletteParentSelector>(RouletteParentSelector());
	auto PMX = std::make_shared<PMXCrossover>(PMXCrossover());
	auto roulNG = std::make_shared<RouletteSelector>(RouletteSelector());

	std::vector<double> mutationsSet{
		0.01, 0.03, 0.05, 0.07, 0.1, 0.15, 0.2
		//0.3, 0.4
	};

	std::vector<Solver*> solvers;
	std::vector<std::string> names;
	auto params = Genetic::Parameters::getStandard();
	params.timeLimit = 10000;
	params.crossover = PMX;
	params.selection = roulNG;
	params.parentSelector = roulPS;
	for (auto mutationValue : mutationsSet) {
		params.mutation = mutationValue;
		solvers.push_back(new Genetic(params));
		int percents = mutationValue * 100;
		names.push_back(std::to_string(percents) + "%");
	}

	runRandomTests(solvers, names, "mutation_stat_long.txt", 40, 40, 1, 5);

	for (int i = 0; i < solvers.size(); i++)
		delete solvers[i];
}


void beginTestForElitism() {
	auto roulPS = std::make_shared<RouletteParentSelector>(RouletteParentSelector());
	auto PMX = std::make_shared<PMXCrossover>(PMXCrossover());
	auto roulNG = std::make_shared<RouletteSelector>(RouletteSelector());

	std::vector<double> elitism{
		0, 0.025, 0.05, 0.075, 0.1, 0.2
		
	};

	std::vector<Solver*> solvers;
	std::vector<std::string> names;
	auto params = Genetic::Parameters::getStandard();
	params.timeLimit = 50000;
	params.crossover = PMX;
	params.selection = roulNG;
	params.parentSelector = roulPS;
	for (auto elit : elitism) {
		params.kidElitism = elit;
		params.parentElitism = elit;
		solvers.push_back(new Genetic(params));
		names.push_back(std::to_string(elit*2));
	}

	runRandomTests(solvers, names, "elit.txt", 40, 40, 1, 5);

	for (int i = 0; i < solvers.size(); i++)
		delete solvers[i];


}

void beginTestForPopulationSize() {
	auto roulPS = std::make_shared<RouletteParentSelector>(RouletteParentSelector());
	auto PMX = std::make_shared<PMXCrossover>(PMXCrossover());
	auto roulNG = std::make_shared<RouletteSelector>(RouletteSelector());

	std::vector<double> popSize{
		8000,15000,20000
	};

	std::vector<Solver*> solvers;
	std::vector<std::string> names;
	auto params = Genetic::Parameters::getStandard();
	params.timeLimit = 10000;
	params.crossover = PMX;
	params.selection = roulNG;
	params.parentSelector = roulPS;
	for (auto size : popSize) {
		params.parentsPopulation = size / 2;
		params.kidsPopulation = size/2;
		solvers.push_back(new Genetic(params));
		names.push_back(std::to_string(size));
	}

	runRandomTests(solvers, names, "population.txt", 40, 40, 1, 5);

	for (int i = 0; i < solvers.size(); i++)
		delete solvers[i];
}


void beginTestsForKnowInstances() {
	auto roulPS = std::make_shared<RouletteParentSelector>(RouletteParentSelector());
	auto randPS = std::make_shared<RandomParentSelector>(RandomParentSelector());
	auto PMX = std::make_shared<PMXCrossover>(PMXCrossover());
	auto optPMX = std::make_shared<PMXLocalOptCrossover>(PMXLocalOptCrossover(1));
	auto roulNG = std::make_shared<RouletteSelector>(RouletteSelector());
	auto randNG = std::make_shared<RandomSelector>(RandomSelector());

	std::vector<std::string> names;
	std::vector<Solver*> solvers;

	/* Standard roullete */
	auto psro = Genetic::Parameters::getStandard();
	psro.parentSelector = roulPS;
	psro.crossover = PMX;
	psro.selection = roulNG;
	Genetic sro(psro);
	solvers.push_back(&sro);
	names.push_back("StdRoul");

	/* Standard random */
	auto psra = Genetic::Parameters::getStandard();
	psra.parentSelector = randPS;
	psra.crossover = PMX;
	psra.selection = randNG;
	Genetic srn(psra);
	solvers.push_back(&srn);
	names.push_back("StdRand");

	/* Standard roull with local opt */
	auto psopt = Genetic::Parameters::getStandard();
	psopt.parentSelector = roulPS;
	psopt.crossover = optPMX;
	psopt.selection = roulNG;
	Genetic sotp(psopt);
	solvers.push_back(&sotp);
	names.push_back("StdOpt");

	/* Only kids */
	auto pokids = Genetic::Parameters::getStandard();
	pokids.parentSelector = roulPS;
	pokids.crossover = PMX;
	pokids.selection = roulNG;
	pokids.includeParents = false;
	pokids.parentElitism = 0.0;
	pokids.kidsPopulation *= 2;
	Genetic okids(pokids);
	solvers.push_back(&okids);
	names.push_back("OnlyKids");

	/* No mutation */
	auto pnomut = Genetic::Parameters::getStandard();
	pnomut.parentSelector = roulPS;
	pnomut.crossover = PMX;
	pnomut.selection = roulNG;
	pnomut.mutation = 0.0;
	Genetic nomut(pnomut);
	solvers.push_back(&nomut);
	names.push_back("NoMut");

	/* No elitism */
	auto pnoelit = Genetic::Parameters::getStandard();
	pnoelit.parentSelector = roulPS;
	pnoelit.crossover = PMX;
	pnoelit.selection = roulNG;
	pnoelit.kidElitism = 0.0;
	pnoelit.parentElitism = 0.0;
	Genetic noelit(pnoelit);
	solvers.push_back(&noelit);
	names.push_back("NoElit");

	/* Greedy */
	Greedy g;
	solvers.push_back(&g);
	names.push_back("Greedy");

	runTests(solvers, names, "know_instances_indep2.txt");
}


void beginTestsForN() {
	auto roulPS = std::make_shared<RouletteParentSelector>(RouletteParentSelector());
	auto PMX = std::make_shared<PMXCrossover>(PMXCrossover());
	auto roulNG = std::make_shared<RouletteSelector>(RouletteSelector());
	/* Standard roullete */
	auto psro = Genetic::Parameters::getStandard();
	psro.parentSelector = roulPS;
	psro.crossover = PMX;
	psro.selection = roulNG;
	psro.timeLimit = 30000;
	Genetic sro(psro);

	std::vector<std::string> names;
	std::vector<Solver*> solvers;


	solvers.push_back(&sro);
	names.push_back("Solver");
	Greedy g;
	solvers.push_back(&g);
	names.push_back("Greedy");

	runRandomTests(solvers, names, "ntest.txt", 20, 1200, 100, 5);
	
	delete solvers[0];
}

int main(int argc, char** argv) {
	std::vector<std::string> args;// {
//		"stuff", "TSPData/gr120.tsp", "4000"
//	};
	for (int i = 0; i < argc; i++) {
		args.push_back(argv[i]);
	}


//	beginTestsForN();
	std::string instancepath = args[1];
	

	auto instance = TSPInstance::loadFromFile(instancepath);
	if (instance->getDistanceMatrix()->size() == 0) {
		std::cout << "Instance not loaded.\n";
		return -1;
	}

	int reference = 1;
	if (args[2].compare("-tour") == 0) {
		

		auto tour = TSPInstance::loadTourFromFile(args[3]);
		reference = instance->calculateGenericSolutionDistance(tour);
	}
	else {
		reference = std::stoi(args[2]);
	}

	std::cout << instancepath << " " << reference;
	
	instance->setReferenceValue(reference);

	auto pars = Genetic::Parameters::getStandard();
	auto roulPS = std::make_shared<RouletteParentSelector>(RouletteParentSelector());
	//auto PMX = std::make_shared<PMXLocalOptCrossover>(PMXLocalOptCrossover(1));
	auto PMX = std::make_shared<PMXCrossover>(PMXCrossover());
	auto roulNG = std::make_shared<RouletteSelector>(RouletteSelector());
	/* Standard roullete */
	pars.parentSelector = roulPS;
	pars.crossover = PMX;
	pars.selection = roulNG;
	pars.timeLimit = 10000;


	Genetic g(pars);

	auto solution = g.solve(instance);
	auto value = instance->calculateGenericSolutionDistance(solution);
	double prd = (double)(value - reference) / (double)(reference);
	std::cout << "Solution: " << instance->calculateGenericSolutionDistance(solution) << " prd=" << prd << "\n";
	for (auto v : solution)
		std::cout << v << " ";

	return 0;
}