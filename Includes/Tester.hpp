#ifndef META_TESTER_H_
#define META_TESTER_H_
#include "Solver.hpp"
#include "KRandom.hpp"
#include "TwoOpt.hpp"
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <ostream>

class Tester
{
public:
	static void runTimeAndPRD(const std::vector<std::shared_ptr<Solver>>& solvers, 
							uint32_t startingInstanceSize, uint32_t step, uint32_t maxInstanceSize, uint32_t testsPerSize,
							const std::string& filepath = "");
	static void runKRandomComparison(uint32_t startingInstanceSize, uint32_t step, uint32_t maxInstanceSize, uint32_t testsPerSize,
							const std::string& filepath = "");
	static void runTimeAndPRDWithTSP(const std::vector<std::shared_ptr<Solver>>& solvers, std::vector<std::string> problems, const std::string& filepath = "");
};

#endif