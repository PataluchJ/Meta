#include "Tester.h"
#include "Greedy.h"
#include "KRandom.h"
#include "TwoOpt.h"

void driver(std::vector<std::string>& arg) {
	/* args: algorithm={greedy|krandom|2opt} file [tour file]*/
	if (arg.size() < 2) {
		std::cerr << "Missing argumens.";
		return;
	}
	Solver* s;
	std::string alg = arg[0];
	size_t offset = 0;
	if (alg.compare("greedy") == 0) {
		s = new Greedy();
	}
	else if (alg.compare("krandom") == 0) {
		size_t k = std::stoi(arg[1]);
		s = new KRandom(k);
		offset = 1;

	}
	else if (alg.compare("2opt") == 0) {
		s = new TwoOpt();
	}
	else {
		std::cerr << "No valid algorithm provided.\n";
		return;
	}

	bool referenceProvided = false;
	bool valueOnly = false;
	auto instance = TSPInstance::loadFromFile(arg[1+ offset]);
	size_t reference;
	if (arg.size() > 2 + offset) {
		referenceProvided = true;

		if (arg[2 + offset].compare("ref") == 0) {
			reference = std::stoi(arg[3 + offset]);
			valueOnly = true;
		}
		else {
			instance->loadTourFromFile(arg[2 + offset]);
			reference = instance->calculateOptimalSolutionDistance();
		}
	}

	s->solve(instance);
	auto solutionDistance = instance->calculateSolutionDistance();
	auto solution = instance->getSolution();
	auto optimalSolution = instance->getOptimalSolution();
	std::cout << "Solution: \n";
	std::copy(solution->begin(), solution->end(), std::ostream_iterator<uint32_t>(std::cout, " "));
	std::cout << "\nDistance: " << solutionDistance << "\n";
	if (referenceProvided) {
		
		if (!valueOnly) {
			std::cout << "Optimal solution: \n";
			std::copy(optimalSolution->begin(), optimalSolution->end(), std::ostream_iterator<uint32_t>(std::cout, " "));
		}
		std::cout << "\nOptimal distance: " << reference << "\n";

		std::cout << "PRD: " << ((double)solutionDistance - (double)reference) / (double)reference;
	}
}

void showRandom(std::vector<std::string>& args) {
	size_t size = std::stoi(args[1]);
	size_t seed = 42;
	size_t min = 0;
	size_t max = 100;
	if(args.size() > 2)
		seed = std::stoi(args[2]);
	if (args.size() > 3)
		min = std::stoi(args[3]);
	if (args.size() > 4)
		max = std::stoi(args[4]);

	InstancePointer instance;
	if (args[0].compare("as") == 0) {
		instance = TSPInstance::generateAsymetricInstance(size, seed,min,max);
	}
	else if(args[0].compare("s") == 0) {
		instance = TSPInstance::generateSymtericInstaance(size, seed, min, max);
	}
	else if (args[0].compare("e") == 0) {
		instance = TSPInstance::generateEuclidInstance(size, seed, min == 0 ? 100 : min);
	}
	else {
		std::cerr << "Not a valid random instance.\n";
	}
	
	auto matrix = instance->getDistanceMatrix();
	std::for_each(matrix->begin(), matrix->end(), [](std::vector<uint32_t>& vec) {
		std::copy(vec.begin(), vec.end(), std::ostream_iterator<uint32_t>(std::cout, " ")); 
		std::cout << "\n"; 
		});
}

int main(int argc, char** argv) {
	std::vector<std::string> args;
	for (int i = 2; i < argc; i++)
		args.push_back(argv[i]);
	if (std::string(argv[1]).compare("rshow") == 0) {
		showRandom(args);
	}
	else if (std::string(argv[1]).compare("tester") == 0) {
		driver(args);
	}
	

	/*
	std::vector<std::string> tspProblems{ "a280", "att48", "bayg29", "bays29", "berlin52",
		"brg180", "ch130", "ch150", "eil51", "eil76", "fri26", "gr24","gr96","gr120","gr666",
		"kroA100", "kroC100","kroD100"};
	
	//Tester::runKRandomComparison(50000, 5000, 50000, 50, "Results\\");
	std::vector<std::shared_ptr<Solver>> solvers{ std::make_shared<Greedy>(Greedy()), std::make_shared<TwoOpt>(TwoOpt())};
	Tester::runTimeAndPRDWithTSP(solvers, tspProblems, "Results\\");
	//Tester t;
	//t.runTimeAndPRD(solvers, 5, 1, 100, 30, "Results\\");
	/*
	auto instance = TSPInstance::loadFromFile("gr120.tsp");
	instance->loadTourFromFile("gr120.opt.tour");
	std::cout << "Optimal solution: " << instance->calculateOptimalSolutionDistance() << "\n";
	//std::unique_ptr<Solver> solver = std::make_unique<Greedy>(Greedy());
	std::unique_ptr<Solver> solver = std::make_unique<TwoOpt>(TwoOpt());
	solver->solve(instance);
	//std::cout << "Greedy: " << instance->calculateSolutionDistance() << "\n";
	//std::cout << "KRandom: " << instance->calculateSolutionDistance() << "\n";
	std::cout << "2Opt: " << instance->calculateSolutionDistance() << "\n";
	//*/
	return 0;
}