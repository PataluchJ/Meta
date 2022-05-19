#include <Greedy.hpp>
#include <TSPInstance.hpp>
#include <iostream>

int main(int argc, char** argv) {
	if (argc < 2)
		return -1;
	Greedy g;
	auto instance = TSPInstance::loadFromFile(argv[1]);
	auto solution = g.solve(instance);
	std::cout << "Solved: " << instance->calculateGenericSolutionDistance(solution) << "\n";
	return 0;
}