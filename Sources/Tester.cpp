#include "Tester.h"

void Tester::runTimeAndPRD(const std::vector<std::shared_ptr<Solver>>& solvers, uint32_t startingInstanceSize, uint32_t step, uint32_t maxInstanceSize, uint32_t testsPerSize, const std::string& filepath)
{
	std::vector<std::vector<double>> PRDs;
	std::vector<std::vector<double>> times;

	std::vector<size_t> temporarySolution;
	std::vector<double> temporaryTimeSum;
	std::vector<double> temporaryPRDSum;

	PRDs.resize(solvers.size());
	times.resize(solvers.size());
	temporarySolution.resize(solvers.size());
	temporaryTimeSum.resize(solvers.size());
	temporaryPRDSum.resize(solvers.size());

	size_t numberOfTests = ((maxInstanceSize - startingInstanceSize) / step );

	std::for_each(PRDs.begin(), PRDs.end(), [numberOfTests](std::vector<double>& v) {v.reserve(numberOfTests); });
	std::for_each(times.begin(), times.end(), [numberOfTests](std::vector<double>& v) {v.reserve(numberOfTests); });

	for (uint32_t instanceSize = startingInstanceSize; instanceSize < maxInstanceSize; instanceSize += step) {
		std::for_each(temporaryPRDSum.begin(), temporaryPRDSum.end(), [](double& d) {d = 0; });
		std::for_each(temporaryTimeSum.begin(), temporaryTimeSum.end(), [](double& d) {d = 0; });
		for (uint32_t test = 0; test < testsPerSize; test++) {
			auto instance = TSPInstance::generateSymtericInstaance(instanceSize, rand());
			for (uint32_t i = 0; i < solvers.size(); i++) {
				auto timeStart = std::chrono::high_resolution_clock::now();
				solvers[i]->solve(instance);
				auto elapsed = std::chrono::high_resolution_clock::now() - timeStart;
				temporaryTimeSum[i] += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
				temporarySolution[i] = instance->calculateSolutionDistance();
			}
			double reference = *std::min_element(temporarySolution.begin(), temporarySolution.end());
			for (uint32_t i = 0; i < solvers.size(); i++)
				temporaryPRDSum[i] += ((double)temporarySolution[i] - reference) / reference;
		}
		for (uint32_t i = 0; i < solvers.size(); i++) {
			PRDs[i].push_back(temporaryPRDSum[i] / (double)testsPerSize);
			times[i].push_back(temporaryTimeSum[i] / (double)testsPerSize);
		}
		std::cout << instanceSize << "/" << maxInstanceSize << "\n";
	}

	for (uint32_t i = 0; i < solvers.size(); i++) {
		std::ofstream timeFile(std::string(filepath + solvers[i]->getName() + "_time.txt"));
		std::ofstream prdFile(std::string(filepath + solvers[i]->getName() + "_prd.txt"));

		if (timeFile.is_open()) {
			std::copy(times[i].begin(), times[i].end(), std::ostream_iterator<double>(timeFile, "\n"));
			timeFile.close();
		}
		else
			std::cerr << "Unable to open " << solvers[i]->getName() << " time file.\n";
		if (prdFile.is_open()) {
			std::copy(PRDs[i].begin(), PRDs[i].end(), std::ostream_iterator<double>(prdFile, "\n"));
			prdFile.close();
		}
		else
			std::cerr << "Unable to open " << solvers[i]->getName() << " prd file.\n";
	}
}

void Tester::runKRandomComparison(uint32_t minimalK, uint32_t step, uint32_t maxK, uint32_t testsPerSize, const std::string& filepath)
{
	std::vector<std::shared_ptr<Solver>> krandoms;
	krandoms.reserve((maxK - minimalK) / step);
	for (uint32_t size = minimalK; size <= maxK; size += step)
		krandoms.push_back(std::make_shared<KRandom>(KRandom(size)));
	krandoms.push_back(std::make_shared<TwoOpt>(TwoOpt()));

	Tester::runTimeAndPRD(krandoms, 10, 10, 130, testsPerSize, filepath);
}

void Tester::runTimeAndPRDWithTSP(const std::vector<std::shared_ptr<Solver>>& solvers, std::vector<std::string> problems, const std::string& filepath)
{
	std::vector<std::vector<double>> PRDs;
	PRDs.resize(solvers.size());

	size_t size = problems.size();
	std::for_each(PRDs.begin(), PRDs.end(), [size](std::vector<double>& d) {d.reserve(size); });

	for (auto& problem : problems) {
		std::cout << "Solving " << problem << "...";
		auto instance = TSPInstance::loadFromFile("TSPData\\" + problem + ".tsp");
		instance->loadTourFromFile("TSPData\\" + problem + ".opt.tour");
		double reference = instance->calculateOptimalSolutionDistance();
		for (size_t i = 0; i < solvers.size(); i++) {
			solvers[i]->solve(instance);
			double solution = instance->calculateSolutionDistance();
			PRDs[i].push_back((solution - reference) / reference);
		}
		std::cout << " done\n";
	}

	std::ofstream out(filepath + "TSPout.txt");
	out << "Problem ";
	for (auto solver : solvers)
		out << " & " << solver->getName();
	out << "\\\\\n";

	for (size_t i = 0; i < problems.size(); i++) {
		out << problems[i];
		for (size_t j = 0; j < solvers.size(); j++) {
			out << " & " << PRDs[j][i];
		}
		out << " \\\\\n";
	}
}
