#include "Tester.h"
#include "Greedy.h"
#include "KRandom.h"
#include "TwoOpt.h"

void driver(int argc, char** argv) {
	/* args: algorithm={greedy|krandom|2opt} file [tour file]*/
	if (argc < 2) {
		std::cerr << "Missing argumens.";
		return;
	}
	Solver* s;
	std::string alg = argv[1];
	if (alg.compare("greedy") == 0) {
		s = new Greedy();
	}
	else if (alg.compare("krandom") == 0) {
		s = new KRandom(1000);
	}
	else if (alg.compare("2opt") == 0) {
		s = new TwoOpt();
	}
	else {
		std::cerr << "No valid algorithm provided.\n";
		return;
	}

	bool referenceProvided = false;
	auto instance = TSPInstance::loadFromFile(argv[2]);
	if (argc > 2) {
		instance->loadTourFromFile(argv[3]);
		referenceProvided = true;
	}

	s->solve(instance);
	auto solutionDistance = instance->calculateSolutionDistance();
	auto solution = instance->getSolution();
	std::cout << "Solution: \n";
	std::copy(solution->begin(), solution->end(), std::ostream_iterator<uint32_t>(std::cout, " "));
	std::cout << "\nDistance: " << solutionDistance << "\n";
	if (referenceProvided) {
		auto reference = instance->calculateOptimalSolutionDistance();
		std::cout << "Optimal solution: \n";
		std::copy(solution->begin(), solution->end(), std::ostream_iterator<uint32_t>(std::cout, " "));
		std::cout << "\nOptimal distance: " << reference << "\n";
		std::cout << "PRD: " << ((double)solutionDistance - (double)reference) / (double)reference;
	}
}

int main(int argc, char** argv) {
	driver(argc, argv);

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