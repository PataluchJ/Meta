#include <Genetic.hpp>

using namespace GA;

std::vector<GA::Pair> GA::RouletteParentSelector::select(Population& population, uint32_t noPairs)
{
	std::vector<Pair> pairs;
	pairs.resize(noPairs);

	/* Generate roulette wheel */
	double sumOfAll = 0.0;
	double minimum = population[0].cost;

	for (auto& spec : population.specimens) {
		if (minimum > spec.cost)
			minimum = spec.cost;
		sumOfAll += spec.cost;
	}

	std::vector<double> partialSums;
	partialSums.resize(population.parentPopulation);
	partialSums[0] = minimum / (double)(population[0].cost);
	for (uint32_t i = 1; i < population.parentPopulation; i++)
		partialSums[i] = partialSums[i - 1] + (minimum / (double)population[i].cost);

	/* Generate pairs */
	auto upperLimit = partialSums.back();
	double scale = upperLimit / (double)(RAND_MAX);
	for (uint32_t i = 0; i < noPairs; i++) {
		pairs[i].a = &(population[findOnWheel(partialSums, (double)(rand()) * scale)]);
		pairs[i].b = &(population[findOnWheel(partialSums, (double)(rand()) * scale)]);
	}

	return pairs;
}

uint32_t GA::RouletteParentSelector::findOnWheel(std::vector<double>& partialSums, double value)
{
	for (uint32_t i = 0; i < partialSums.size(); i++) {
		if (partialSums[i] >= value)
			return i;
	}
	return partialSums.size();
}

std::vector<GA::Pair> GA::RandomParentSelector::select(Population& population, uint32_t noPairs)
{
	std::vector<Pair> pairs;
	pairs.resize(noPairs);
	for (uint32_t i = 0; i < noPairs; i++) {
		pairs[i].a = &(population[rand() % population.parentPopulation]);
		pairs[i].b = &(population[rand() % population.parentPopulation]);
	}

	return pairs;
}

void GA::PMXCrossover::crossover(const Specimen* pa, const Specimen* pb, Specimen* ka, Specimen* kb, float mutation)
{
	/*
	uint32_t bestPivot = 0;
	uint64_t bestPivotCost = UINT64_MAX;
	for (int i = 1; i < pa->solution.size()-1; i++) {
		uint32_t pivot = i;
		*ka = *pa;
		*kb = *pb;
		pmxSwap(pa, kb, pivot);
		pmxSwap(pb, ka, pivot);

		
		auto aCost = ins->calculateGenericSolutionDistance((ka->solution));
		auto bCost = ins->calculateGenericSolutionDistance((kb->solution));

		auto cost = aCost * bCost;
		if (cost < bestPivot) {
			bestPivot = pivot;
		}

	}

	*ka = *pa;
	*kb = *pb;
	pmxSwap(pa, kb, bestPivot);
	pmxSwap(pb, ka, bestPivot);
	*/

	uint32_t pivot = (rand() % (pa->solution.size() - 3)) + 1;
	*ka = *pa;
	*kb = *pb;
	pmxSwap(pa, kb, pivot);
	pmxSwap(pb, ka, pivot);
	
	if ((double)(rand() % 100) / 100.0 <= mutation) {
		std::shuffle(ka->solution.begin(), ka->solution.end(), std::default_random_engine(rand()));
	}
	if ((double)(rand() % 100) / 100.0 <= mutation) {
		std::shuffle(kb->solution.begin(), kb->solution.end(), std::default_random_engine(rand()));
	}
}

void GA::PMXLocalOptCrossover::crossover(const Specimen* pa, const Specimen* pb, Specimen* ka, Specimen* kb, float mutation)
{
	uint8_t pivot = (rand() % (pa->solution.size() - 3)) + 1;
	*ka = *pa;
	*kb = *pb;
	pmxSwap(pa, kb, pivot);
	pmxSwap(pb, ka, pivot);
	if ((double)(rand() % 100) / 100.0 <= mutation) 
		std::shuffle(ka->solution.begin(), ka->solution.end(), std::default_random_engine(rand()));
	else
		optimizer->optimize(ka->solution);
	if ((double)(rand() % 100) / 100.0 <= mutation) 
		std::shuffle(kb->solution.begin(), kb->solution.end(), std::default_random_engine(rand()));
	else 
		optimizer->optimize(kb->solution);
}


void GA::RouletteSelector::select(Population& population, double elitism, bool includeParents)
{
	uint32_t size = population.size();
	if (!includeParents) {
		for (uint32_t i = 0; i < population.parentPopulation; i++) {
			population[i].cost = UINT64_MAX;
		}
		size -= population.parentPopulation;
	}

	std::sort(population.specimens.begin(), population.specimens.end(), [](const Specimen& a, const Specimen& b) {
		return a.cost < b.cost;
		});
	uint32_t offset = population.parentPopulation * elitism;

	/* Generate roulette wheel */
	double minimum = population[0].cost;

	for (auto& spec : population.specimens) {
		if (minimum >= spec.cost)
			minimum = spec.cost;
	}
	
	std::vector<double> partialSums;
	/* Select new speciments by moving them to the front */
	while (offset < population.parentPopulation) {
		/* Regenerate wheel */
		partialSums.clear();
		partialSums.reserve(population.size());
		partialSums.push_back(minimum / (double)(population[offset].cost));
		for (uint32_t i = 1; i+offset < population.size() && population[i + offset].cost != UINT64_MAX; i++)
			partialSums.push_back(partialSums[i - 1] + (minimum / (double)population[i + offset].cost));

		auto specIndex = spinTheWheel(partialSums, 0) + offset;
		std::swap(population[specIndex], population[offset]);

		offset++;
	}

	/* Remove speciments */
	for (int i = offset; i < population.size(); i++) {
		population[i].cost = UINT64_MAX;
	}
}

uint32_t GA::RouletteSelector::spinTheWheel(std::vector<double>& wheel, double sum)
{
	double scale = wheel.back() / (double)(RAND_MAX);
	double point = (double)(rand()) * scale;

	for (uint32_t i = 0; i < wheel.size(); i++) {
		if (wheel[i] >= point)
			return i;
	}

	return wheel.size();
}

void GA::pmxSwap(const Specimen* parent, Specimen* kid, uint32_t pivot)
{
	for (uint32_t i = 0; i < pivot; i++) {
		/* Swap in kid[i] with kid[j] where kid[j] == parent[i] */
		for (uint32_t j = 0; j < parent->solution.size(); j++) {
			if (kid->solution[j] == parent->solution[i]) {
				std::swap(kid->solution[j], kid->solution[i]);
				break;
			}
		}
	}
}

Solution GA::Genetic::solve(InstancePointer instance)
{
	Logger::logInfo("Start");
	/* Run Statistic */
	RunStatistic rs;
	rs.start = std::chrono::high_resolution_clock::now();
	rs.iteration = 0;
	rs.stagnation = 0;
	Population population;

	population.specimens.resize(parameters.parentsPopulation + parameters.kidsPopulation);
	population.nextGenerationOffset = parameters.parentsPopulation;
	population.parentPopulation = parameters.parentsPopulation;
	population.populationSize = parameters.kidsPopulation + parameters.parentsPopulation;

	/* Fill generation with random speciments */
	// TODO: Insert some betters speciments
	auto solutionSize = instance->getDistanceMatrix()->size();
	generateRandomPopulation(instance, population, solutionSize);

	Solution bestSolution = population[0].solution;
	uint64_t bestSolutionCost = population[0].cost;

	while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - rs.start).count() < parameters.timeLimit) {
		//if (rs.iteration % 10 == 0)
		//	Logger::logInfo("#");
		rs.iteration++;
		rs.stagnation++;
		/* Generate pairs */
		auto parents = parameters.parentSelector->select(population, parameters.kidsPopulation/2);

		/* Generate kids */
		uint32_t nextGenVectorIndex = population.nextGenerationOffset;
		for (auto& pair : parents) {
			parameters.crossover->crossover(pair.a, pair.b, &(population[nextGenVectorIndex]), &population[nextGenVectorIndex], parameters.mutation);
			nextGenVectorIndex += 2;
		}

		/* Calculate distance for kids */
		for (uint32_t i = population.nextGenerationOffset; i < population.size(); i++)
			population[i].cost = instance->calculateGenericSolutionDistance(population[i].solution);

		/* Select next gen */
		parameters.selection->select(population, parameters.elitism, parameters.includeParents);

		/* Check for new best solution */
		for (auto& spec : population.specimens) {
			if (spec.cost < bestSolutionCost) {
				//double prd = (((double)bestSolutionCost - (double)spec.cost) / (double)bestSolutionCost) * 100;
				//Logger::logInfo(std::to_string(rs.iteration) + "\t" + std::to_string(bestSolutionCost) + "\t" + std::to_string(prd) + "%");
				bestSolution = spec.solution;
				bestSolutionCost = spec.cost;
				rs.stagnation = 0;
			}
		}
	}
	Logger::logInfo("Finished with " + std::to_string(rs.iteration));
	return bestSolution;
}

void GA::Genetic::generateRandomPopulation(InstancePointer instance, Population& population, uint32_t size)
{
	Specimen starting;
	starting.solution.resize(size);
	for (auto i = 0; i < size; i++)
		starting.solution[i] = i;

	auto randEngine = std::default_random_engine(0);
	for (auto& spec : population.specimens) {
		spec.solution = starting.solution;
		std::shuffle(spec.solution.begin(), spec.solution.end(), randEngine);
		spec.cost = instance->calculateGenericSolutionDistance(spec.solution);
	}
}

void GA::RandomSelector::select(Population& population, double elitims, bool includeParents)
{
	uint32_t size = population.size();
	if (!includeParents) {
		for (uint32_t i = 0; i < population.parentPopulation; i++)
			population[i].cost = UINT64_MAX;
		size -= population.parentPopulation;
	}
	std::sort(population.specimens.begin(), population.specimens.end(), [](const Specimen& a, const Specimen& b) {
		return a.cost < b.cost;
		});
	
	uint32_t i = population.parentPopulation * elitims;
	for (; i < population.parentPopulation; i++) {
		std::swap(population[i], population[rand() % (size - i - 1) + (i + 1)]);
	}

	for (; i < population.size(); i++)
		population[i].cost = UINT64_MAX;
}
