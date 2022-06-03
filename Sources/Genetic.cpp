#include "Genetic.h"

using namespace GA;

std::vector<GA::Pair> GA::RouletteParentSelector::select(Population& population, uint32_t noPairs)
{
	std::vector<Pair> pairs;
	pairs.resize(noPairs);

	/* Generate roulette wheel */
	double sumOfAll = 0.0;
	double minimum = population[0].cost;

	for (auto& spec : population.specimens) {
		if (minimum >= spec.cost)
			minimum = spec.cost;
		sumOfAll += spec.cost;
	}

	std::vector<double> partialSums;
	partialSums.resize(population.size());
	partialSums[0] = minimum / (double)(population[0].cost);
	for (uint32_t i = 1; i < population.size(); i++)
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
		if (partialSums[i] <= value)
			return i;
	}
	return partialSums.size();
}

std::vector<GA::Pair> GA::RandomParentSelector::select(Population& population, uint32_t noPairs)
{
	std::vector<Pair> pairs;
	pairs.resize(noPairs);
	for (uint32_t i = 0; i < noPairs; i++) {
		pairs[i].a = &(population[rand() % population.size()]);
		pairs[i].b = &(population[rand() % population.size()]);
	}

	return pairs;
}

void GA::PMXCrossover::crossover(const Specimen* pa, const Specimen* pb, Specimen* ka, Specimen* kb, float mutation)
{
	uint32_t pivot = (rand() % (pa->solution.size() - 3)) + 1;
	*ka = *pa;
	*kb = *pb;
	pmxSwap(pa, kb, pivot);
	pmxSwap(pb, ka, pivot);
}

void GA::PMXLocalOptCrossover::crossover(const Specimen* pa, const Specimen* pb, Specimen* ka, Specimen* kb, float mutation)
{
	uint8_t pivot = (rand() % (pa->solution.size() - 3)) + 1;
	*ka = *pa;
	*kb = *pb;
	pmxSwap(pa, kb, pivot);
	pmxSwap(pb, ka, pivot);
	optimizer.optimize(ka->solution);
	optimizer.optimize(kb->solution);
}


void GA::RouletteSelector::select(Population& population, uint32_t desiredPopulation, double elitism)
{
	std::sort(population.specimens.begin(), population.specimens.end(), [](const Specimen& a, const Specimen& b) {
		return a.cost < b.cost;
		});
	uint32_t offset = 0;
	offset = population.size() * elitism;

	/* Generate roulette wheel */
	double sumOfAll = 0.0;
	double minimum = population[0].cost;

	for (auto& spec : population.specimens) {
		if (minimum >= spec.cost)
			minimum = spec.cost;
		sumOfAll += spec.cost;
	}

	std::vector<double> partialSums;
	partialSums.resize(population.size());
	partialSums[0] = minimum / (double)(population[0].cost);
	for (uint32_t i = 1; i < population.size(); i++)
		partialSums[i] = partialSums[i - 1] + (minimum / (double)population[i].cost);
	
	/* Remove speciments that passed from elitism param. */
	for (int i = 0; i < offset; i++)
		partialSums[i] = 0.0;

	/* Select new speciments by moving them to the front */
	while (offset < desiredPopulation) {
		auto specIndex = spinTheWheel(partialSums);
		partialSums[specIndex] = 0.0; // Remove spec. from wheel 
		std::swap(partialSums[specIndex], partialSums[offset]);
		std::swap(population[specIndex], population[offset]);
		offset++;
	}

	/* Remove speciments */
	for (int i = offset; i < population.size(); i++) {
		population[i].cost = UINT64_MAX;
	}
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
	/* Run Statistic */
	RunStatistic rs;
	rs.start = std::chrono::high_resolution_clock::now();
	Population population;
	if (parameters.populationSize % 2 != 0)
		parameters.populationSize++;
	population.specimens.resize(parameters.populationSize);
	population.nextGenerationOffset = parameters.populationSize / 2;

	/* Fill generation with random speciments */
	// TODO: Insert some betters speciments
	auto solutionSize = instance->getDistanceMatrix()->size();
	generateRandomPopulation(instance, population, solutionSize);

	Solution bestSolution = population[0].solution;
	uint64_t bestSolutionCost = population[0].cost;

	while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - rs.start) < parameters.timeLimit) {
		rs.iteration++;
		rs.stagnation++;
		/* Generate pairs */
		auto parents = parameters.parentSelector->select(population, parameters.populationSize);

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
		parameters.selection->select(population, parameters.populationSize, parameters.elitism);

		/* Check for new best solution */
		for (auto& spec : population.specimens) {
			if (spec.cost < bestSolutionCost) {
				bestSolution = spec.solution;
				bestSolutionCost = spec.cost;
				Logger::logInfo("New best " + std::to_string(bestSolutionCost));
				rs.stagnation = 0;
			}
		}
	}
	return bestSolution;
}

void GA::Genetic::generateRandomPopulation(InstancePointer instance, Population& population, uint32_t size)
{
	Specimen starting;
	starting.solution.resize(size);
	for (auto i = 0; i < size; i++)
		starting.solution[i] = i;

	std::for_each(
		population.specimens.begin(),
		population.specimens.end(),
		[&starting, &instance](Specimen& spec) {
			spec.solution = starting.solution;
			std::shuffle(spec.solution.begin(), spec.solution.end(), std::default_random_engine());
			spec.cost = instance->calculateGenericSolutionDistance(spec.solution);
		});
}
