#include <iostream>

#include "TwoOpt.h"
#include "Greedy.h"
#include "TabuSearch.h"
#include "TSPInstance.h"

constexpr uint32_t startingSize = 20;
constexpr uint32_t endSize = 1620;
constexpr uint32_t step = 200;

constexpr uint32_t testsPerSize = 15;
constexpr uint32_t testNumber = (((endSize - startingSize) / step) + 1) * testsPerSize;
constexpr uint32_t knowInstanceCount = 32;

void runTestsOnRandomInstances(std::vector<std::shared_ptr<Solver>> solvers,std::vector<std::string>& solverNames, std::string filename) {
	

	std::cout << "Starting tests for random instances. Saving in \"" << filename << "\"\n";

	/* Set output */
	std::streambuf* buf;
	std::ofstream statFile(filename, std::ios::out);
	if (!statFile.is_open()) {
		std::cerr << "#Unable to open file for saving statistic. Printing to console!\n";
		buf = std::cout.rdbuf();
	}
	else {
		buf = statFile.rdbuf();
	}
	std::ostream out(buf);

	/* Header */
	out << "Size";
	for (auto name : solverNames)
		out << ";" << name;
	out << "\n";

	for (auto size = startingSize; size <= endSize; size += step) {
		for (auto testNo = 0; testNo < testsPerSize; testNo++) {
			auto instance = TSPInstance::generateSymtericInstaance(size, size*testNo);
			out << size;
			/* Test solvers */
			for (auto& solver : solvers) {
				auto solution = solver->solve(instance);
				out << ";" << instance->calculateGenericSolutionDistance(solution);
			}
			out << "\n";
			
		}
		std::cout << "Done: " << size << "\\" << endSize << "\n";
	}
}

void runTestsOnKnowInstances(std::vector<std::shared_ptr<Solver>>& solvers, std::vector<std::string>& solverNames, std::string filename){
	std::vector<std::string> filenames{"a280", "att48", "bayg29", "bays29",
		"berlin52", "brg180", "ch130", "ch150", "eil101", "eil51", "eil76",
		"fri26", "gr120", "gr202", "gr24", "gr48", "gr666", "gr96", "kroA100",
		"kroC100", "kroD100", "lin105", "pa561", "pcb442", "pr1002", "pr2392",
		"pr76", "rd100", "st70", "tsp225", "ulysses16", "ulysses22" };
	
	std::cout << "Starting tests for know instances. Saving in: \"" << filename << "\"\n";
	std::string path = "TSPData\\";
	/* Set output */
	std::streambuf* buf;
	std::ofstream statFile(filename, std::ios::out);
	if (!statFile.is_open()) {
		std::cerr << "Unable to open file for saving statistic. Printing to console!\n";
		buf = std::cout.rdbuf();
	}
	else {
		buf = statFile.rdbuf();
	}
	std::ostream out(buf);

	/* Header */
	out << "Instance;Optimal";
	for (auto name : solverNames)
		out << ";" << name;
	out << "\n";

	try {
		for (auto& file : filenames) {
			auto instance = TSPInstance::loadFromFile(path + file + ".tsp");
			if (instance == nullptr) {
				std::cerr << "#Unable to load " << file << ", skipped.\n";
				continue;
			}
			
			auto solution = TSPInstance::loadTourFromFile(path + file + ".opt.tour");
			if (solution.size() == 0) {
				std::cerr << "#Unable to load " << file << ".opt.tour, skipped.\n";
				continue;
			}
			std::cout << "Testing " << file << "\n";
			auto optimal = instance->calculateGenericSolutionDistance(solution);

			out << file << ";" << optimal;

			/* Test solvers */
			for (auto& solver : solvers) {
				solution = solver->solve(instance);
				out << ";" << instance->calculateGenericSolutionDistance(solution);
			}
			out << "\n";
		}
	}
	catch (std::exception& e) {
		std::cerr << "#Exception has occured!\n";
	}

	std::cout << "Tests ended.\n";
}

void testTabuLen() {
	TabuSearch::TabuSearchConfig root;
	root.tabuLength = TabuSearch::TabuLenght::Root;
	TabuSearch::TabuSearchConfig normal;
	normal.tabuLength = TabuSearch::TabuLenght::Normal;
	TabuSearch::TabuSearchConfig square;
	square.tabuLength = TabuSearch::TabuLenght::Square;
	std::vector<std::shared_ptr<Solver>> solvers{ 
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{2000}, root),
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{2000}, normal),
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{2000}, square),
	
	};
	std::vector<std::string> names{ "Root", "Normal", "Square"};
	std::string file = "Results/tabuLength";

	constexpr auto randomTime = testNumber * 3 * 2000;
	constexpr auto knowTime = knowInstanceCount * 3 * 2000;
	std::cout << "Testing tabu lenght. Expected time for random: " << randomTime / 1000.0 << " seconds. For know instances : " << knowTime / 1000.0 << " seconds.\n";
	runTestsOnRandomInstances(solvers, names, file+".rand.txt");
	runTestsOnKnowInstances(solvers, names, file+".know.txt");
}

void testNeighborhoodType() {
	TabuSearch::TabuSearchConfig insert;
	insert.neighborhoodType = Neighborhood::NeighborhoodFunction::Insert;
	TabuSearch::TabuSearchConfig reverse;
	reverse.neighborhoodType = Neighborhood::NeighborhoodFunction::Reverse;
	TabuSearch::TabuSearchConfig swap;
	swap.neighborhoodType = Neighborhood::NeighborhoodFunction::Swap;
	std::vector<std::shared_ptr<Solver>> solvers{
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{2000}, insert),
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{2000}, reverse),
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{2000}, swap),

	};
	std::vector<std::string> names{ "Insert", "Reverse", "Swap" };
	std::string file = "Results/neighborhoodType";
	constexpr auto randomTime = testNumber * 3 * 2000;
	constexpr auto knowTime = knowInstanceCount * 3 * 2000;
	std::cout << "Testing neightborhood type. Expected time for random: " << randomTime / 1000.0 << " seconds. For know instances : " << knowTime / 1000.0 << " seconds.\n";
	runTestsOnRandomInstances(solvers, names, file + ".rand.txt");
	runTestsOnKnowInstances(solvers, names, file + ".know.txt");
}

void testHistoryLength() {
	TabuSearch::TabuSearchConfig history;
	history.historyLength = UINT32_MAX;
	TabuSearch::TabuSearchConfig noHistory;
	noHistory.historyLength = 0;
	std::vector<std::shared_ptr<Solver>> solvers{
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{2000}, history),
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{2000}, noHistory),

	};
	std::vector<std::string> names{ "Unlimited", "None" };
	std::string file = "Results/historyLenght";
	constexpr auto randomTime = testNumber * 2 * 2000;
	constexpr auto knowTime = knowInstanceCount * 2 * 2000;
	std::cout << "Testing neightborhood type. Expected time for random: " << randomTime / 1000.0 << " seconds. For know instances : " << knowTime / 1000.0 << " seconds.\n";
	runTestsOnRandomInstances(solvers, names, file + ".rand.txt");
	runTestsOnKnowInstances(solvers, names, file + ".know.txt");
}

void testTime() {
	TabuSearch::TabuSearchConfig config;
	std::vector<std::shared_ptr<Solver>> solvers{
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{1000}, config),
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{6000}, config),
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{60000}, config),

	};
	std::vector<std::string> names{ "Second", "2 Sec", "1 Min"};
	std::string file = "Results/time";
	constexpr auto knowTime = knowInstanceCount * 67000;
	std::cout << "Testing neightborhood type. Expected time for know instances : " << knowTime / 1000.0 << " seconds.\n";
	runTestsOnKnowInstances(solvers, names, file + ".know.txt");


}

void testSolve() {
	TabuSearch::TabuSearchConfig config;
	config.neighborhoodType = Neighborhood::NeighborhoodFunction::Swap;
	TabuSearch ts(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{ 6000 }, config);
	auto instance = TSPInstance::loadFromFile("gr120.tsp");
	auto solution = ts.solve(instance);
	auto optimal = TSPInstance::loadTourFromFile("gr120.opt.tour");

	std::cout << "Distance: " << instance->calculateGenericSolutionDistance(solution) << "\n";
	for (auto v : solution)
		std::cout << v << " ";
	std::cout << "\nOptimal: " << instance->calculateGenericSolutionDistance(optimal) << "\n";
	for (auto v : optimal)
		std::cout << v << " ";

}

void neightboortypeTest() {
	Solution solution{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13 };
	Neighborhood n(solution, Neighborhood::NeighborhoodFunction::Insert);
	auto it = n.begin();
	for (int i = 0; i < 5; i++) {
		std::cout << it.getCurrentMove().l << " " << it.getCurrentMove().r << ":\n";
		auto v = *it;
		for (int j = 0; j < 10; j++) {
			std::cout << v[j] << " ";
		}
		std::cout << "\n";
		it++;
	}
}

void wilcoxon() {
	TabuSearch::TabuSearchConfig config;

	std::vector<std::shared_ptr<Solver>> solvers{
		std::make_shared<TabuSearch>(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{2000}, config),
		std::make_shared<TwoOpt>(TwoOpt())
	};
	std::vector<std::string> names{ "Tabu", "TwoOpt" };
	std::string file = "Results/wilcxon";
	std::cout << "Testing wilcoxon. \n";
	runTestsOnRandomInstances(solvers, names, file + ".rand.txt");
}

void testSolve(std::string instanceFile, std::string tourFile, uint32_t time, uint64_t reference = 0) {
	auto instance = TSPInstance::loadFromFile(instanceFile);
	if (instance == nullptr) {
		std::cerr << "Unable to open file!\n";
		return;
	}
	uint64_t optimalDistance = reference;
	if (optimalDistance == 0) {
		if (tourFile.compare("") != 0) {
			auto optimalSolution = TSPInstance::loadTourFromFile(tourFile);
			if (optimalSolution.size() == 0) {
				std::cerr << "Unable to open tour file!\n";
				return;
			}
			optimalDistance = instance->calculateGenericSolutionDistance(optimalSolution);
		}
	}
	TabuSearch::TabuSearchConfig config;
	TabuSearch ts(std::make_shared<Greedy>(Greedy()), TabuSearch::Time{ time }, config);
	ts.setPRDReference(optimalDistance);
	auto solution = ts.solve(instance);
	
	auto distance = instance->calculateGenericSolutionDistance(solution);
	std::cout << "Final solution distance = " << distance;
	if (optimalDistance != 0) {
		std::cout << " PRD = " << ((double)distance - (double)optimalDistance) / (double)(optimalDistance) << "\n";
		std::cout << "Optimal solution distance: " << optimalDistance << "\n";
	}
	std::cout << "Solution: ";
	for (auto& v : solution) {
		std::cout << v << " ";
	}
	std::cout << "\n";
}

int main(int argc, char** argv) {
	//testTabuLen();
	//testNeighborhoodType();
	//testHistoryLength();
	//testSolve();
	//wilcoxon();
	std::string instance = argv[1];
	std::string opttour = "";
	uint32_t time = 2000;
	uint64_t reference = 0;
	int offset = 0;
	if (argc > 2) {
		opttour = argv[2];
		if (opttour.compare("const") == 0) {
			offset++;
			reference = std::stoi(argv[3]);
		}
	}
	if (argc > 3+ offset)
		time = std::stoi(argv[3+ offset]);
	
	testSolve(instance , opttour, time, reference);
	return 0;
}