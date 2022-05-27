#include "Genetic.h"

using namespace Gen;

Mating::Function Mating::getRandom()
{
    return [](PopulationPointer population, uint32_t numberOfPairs) -> SelectedParents&& {
		SelectedParents pairs;
		pairs.resize(numberOfPairs);
		for (uint32_t i = 0; i < numberOfPairs; i++) {
			pairs[i].a = &((*population)[rand() % population->size()]);
			pairs[i].b = &((*population)[rand() % population->size()]);
		}

		return std::move(pairs);
	};
}

Mating::Function Mating::getRoulette()
{
	return [](PopulationPointer population, uint32_t numberOfPairs) -> SelectedParents&& {
		SelectedParents pairs;
		pairs.resize(numberOfPairs);

		/* Create roulette 'wheel' */
		double sumOfAll = 0;
		double minimum = (*population)[0].cost;
		for (auto& spec : *population) {
			if (minimum > spec.cost)
				minimum = spec.cost;
			sumOfAll += spec.cost;
		}
		std::vector<double> partialSums;
		partialSums.resize(population->size());

		partialSums[0] = minimum / (double)(*population)[0].cost;
		for (uint32_t i = 1; i < population->size(); i++) {
			partialSums[i] = partialSums[i - 1] + (minimum / (double)(*population)[i].cost);
		}

		auto findSpecimenInRulette = [&partialSums](double value) -> uint32_t {
			for (uint32_t i = 0; i < partialSums.size(); i++) {
				if (partialSums[i] <= value)
					return i;
			}
			return partialSums.size();
		};

		/* Randomize parents with prob. related to their cost (less is better) */
		auto upperLimit = partialSums.back();
		double scale = upperLimit / (double)(RAND_MAX);
		for (uint32_t i = 0; i < numberOfPairs; i++) {
			pairs[i].a = &((*population)[findSpecimenInRulette(
				((double)rand()) * scale
			)]);
			pairs[i].b = &((*population)[findSpecimenInRulette(
				((double)rand()) * scale
			)]);
		}

		return std::move(pairs);
	};
}

Crossover::Function Crossover::getPMX()
{
	return [](const Specimen* pa, const Specimen* pb, Specimen* ka, Specimen* kb, float mutation) {
		uint8_t pivot = (rand() % (pa->solution.size() - 3)) + 1;
		*ka = *pa;
		*kb = *pb;
		pmxSwap(pa, kb, pivot);
		pmxSwap(pb, ka, pivot);
	};
}

Crossover::Function Crossover::getPMXWithLocalOpt(OptymalizerPointer optimizer)
{
	return [&optimizer](const Specimen* pa, const Specimen* pb, Specimen* ka, Specimen* kb, float mutation) {
		uint8_t pivot = (rand() % (pa->solution.size() - 3)) + 1;
		*ka = *pa;
		*kb = *pb;
		pmxSwap(pa, kb, pivot);
		pmxSwap(pb, ka, pivot);
		optimizer->optimize(ka->solution);
		optimizer->optimize(kb->solution);
	};
}

void Crossover::pmxSwap(const Specimen* parent, Specimen* kid, uint8_t pivot)
{
	for (uint8_t i = 0; i < pivot; i++) {
		/* Swap in kid[i] with kid[j] where kid[j] == parent[i] */
		for(uint8_t j = 0; j < parent->solution.size(); j++) {
			if (kid->solution[j] == parent->solution[i]) {
				std::swap(kid->solution[j], kid->solution[i]);
				break;
			}
		}
	}
}

Selection::Function Selection::getRoulette()
{
	return [](PopulationPointer parents, PopulationPointer kids, uint32_t desiredPopulation, float selectionElitism) {
		
	};
}

Solution Genetic::solve(InstancePointer instance)
{
	/* Create containters for generations */
	PopulationPointer currentGeneration = std::make_shared<Population>(Population());
	PopulationPointer nextGeneration = std::make_shared<Population>(Population());

	currentGeneration->resize(params.desiredPopulation);
	nextGeneration->resize(params.desiredPopulation);

	/* Fill generation with random speciments */
	// TODO: Insert some betters speciments
	auto solutionSize = instance->getDistanceMatrix()->size();
	generateRandomPopulation(instance, currentGeneration, solutionSize);

	// TODO: end condition 
	while (true) {

		/* Generate pairs */
		auto parents = params.mate(currentGeneration, params.desiredPopulation);

		/* Generate kids */
		uint32_t nextGenVectorIndex = 0;
		for (auto& pair : parents) {
			params.crossover(pair.a, pair.b, &(*nextGeneration)[nextGenVectorIndex], &(*nextGeneration)[nextGenVectorIndex + 1], params.currentMutation);
			nextGenVectorIndex += 2;
		}
		/* Calculate distance for kids */
		for (auto& kid : *nextGeneration)
			kid.cost = instance->calculateGenericSolutionDistance(kid.solution);
		

		/* Select next gen */
		params.selection(currentGeneration, nextGeneration, params.desiredPopulation, params.elitism);

	}
	return Solution();
}

void Gen::Genetic::generateRandomPopulation(InstancePointer instance, PopulationPointer population, uint32_t size)
{
	Specimen starting;
	starting.solution.resize(size);
	for (auto i = 0; i < size; i++)
		starting.solution[i] = i;

	std::for_each(
		population->begin(),
		population->end(),
		[&starting, &instance](Specimen& spec) {
			spec.solution = starting.solution;
			std::shuffle(spec.solution.begin(), spec.solution.end(), std::default_random_engine());
			spec.cost = instance->calculateGenericSolutionDistance(spec.solution);
		}
	);
}

Gen::Selection::CombinePointers::CombinePointers(PopulationPointer a, PopulationPointer b)
	: a(a), b(b)
{
	bStart = a->size();
}

Specimen& Gen::Selection::CombinePointers::operator[](size_t index)
{
	if (index < bStart)
		return (*a)[index];
	return (*b)[index - bStart];
}
