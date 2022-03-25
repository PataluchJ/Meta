#include "TSPInstance.h"

TSPInstance::TSPInstance(size_t size)
{
	/* Set pointers */
	this->adjacencyMatrix = DistanceMatrixPointer(new DistanceMatrix);
	this->solution = SolutionPointer(new Solution);
	this->optimalSolution = SolutionPointer(new Solution);
	/* Resize containers */
	this->solution->resize(size, 0);
	this->optimalSolution->resize(size, 0);
	this->adjacencyMatrix->resize(size);
	for (auto& subVector : *(this->adjacencyMatrix))
		subVector.resize(size, 0);
}

TSPInstance::~TSPInstance()
{
}

DistanceMatrixPointer TSPInstance::getDistanceMatrix()
{
	return this->adjacencyMatrix;
}

SolutionPointer TSPInstance::getSolution()
{
	return this->solution;
}

SolutionPointer TSPInstance::getOptimalSolution()
{
	return this->optimalSolution;
}

uint64_t TSPInstance::calculateSolutionDistance()
{
	return this->calculateGenericSolutionDistance(this->solution);
}

uint64_t TSPInstance::calculateOptimalSolutionDistance()
{
	return this->calculateGenericSolutionDistance(this->optimalSolution);
}

InstancePointer TSPInstance::generateAsymetricInstance(size_t size, unsigned int seed, uint32_t minimumDistance, uint32_t maximumDistance)
{
	InstancePointer instance = InstancePointer(new TSPInstance(size));
	auto matrix = *(instance->getDistanceMatrix());
	auto modulo = maximumDistance - minimumDistance;
	srand(seed);
	for (size_t v = 0; v < size; v++) 
		for (size_t u = 0; u < size; u++) 
			matrix[v][u] = rand() % modulo + minimumDistance;

	for (size_t v = 0; v < size; v++)
		matrix[v][v] = 0;

	return instance;
}

InstancePointer TSPInstance::generateSymtericInstaance(size_t size, unsigned int seed, uint32_t minimumDistance, uint32_t maximumDistance)
{
	InstancePointer instance = InstancePointer(new TSPInstance(size));
	auto matrix = *(instance->getDistanceMatrix());
	auto modulo = maximumDistance - minimumDistance;
	srand(seed);
	for (size_t v = 0; v < size; v++) {
		for (size_t u = v + 1; u < size; u++) {
			uint32_t distance = rand() % modulo + minimumDistance;
			matrix[v][u] = distance;
			matrix[u][v] = distance;
		}
	}

	return instance;
}

InstancePointer TSPInstance::generateEuclidInstance(size_t size, unsigned int seed, uint32_t minimumDistance, uint32_t maximumDistance )
{
	std::vector<float> xCoord;
	std::vector<float> yCoord;
	xCoord.resize(size, 0);
	yCoord.resize(size, 0);
	
	/* Generate points */
	double divider = static_cast<double>(RAND_MAX / (maximumDistance - minimumDistance));
	for (size_t i = 0; i < size; i++) {
		xCoord[i] = minimumDistance + static_cast<double>(rand() / divider);
		yCoord[i] = minimumDistance + static_cast<double>(rand() / divider);
	}
	
	/* Create distance matrix */
	InstancePointer instance = InstancePointer(new TSPInstance(size));
	auto matrix = *(instance->getDistanceMatrix());
	for (size_t v = 0; v < size; v++) {
		for (size_t u = v+1; u < size; u++) {
			uint32_t distance = (uint32_t)(sqrt((xCoord[v] - xCoord[u]) * (xCoord[v] - xCoord[u]) +
				(yCoord[v] - yCoord[u]) * (yCoord[v] - yCoord[u])) + 0.5);
			
			matrix[v][u] = distance;
			matrix[u][v] = distance;
		}
	}
	
	return instance;
}

InstancePointer TSPInstance::loadFromFile(const std::string& filepath)
{
	TSPFile file(filepath);
	if (!file.isGood()) {
		std::cerr << "Unable to load instance from file.\n";
		return nullptr;
	}
	InstancePointer instance(new TSPInstance(file.dimension));

	auto target = instance->adjacencyMatrix;
	auto source = file.matrix;

	for (size_t i = 0; i < file.dimension; i++) {
		for (size_t j = 0; j < file.dimension; j++) {
			(*target)[i][j] = (uint32_t)(source[i][j]);
		}
	}

	return instance;
}

bool TSPInstance::loadTourFromFile(const std::string& filepath)
{
	TSPFile file(filepath);
	if (!file.isGood()) {
		std::cerr << "Unable to load instance from file.\n";
		return false;
	}

	optimalSolution->resize(file.dimension, 0);
	if (file.tour.size() != file.dimension) {
		std::cerr << "File does not contain tour section!";
		return false;
	}

	for (size_t i = 0; i < file.dimension; i++) {
		(*optimalSolution)[i] = file.tour[i];
	}

	return true;
}

uint64_t TSPInstance::calculateGenericSolutionDistance(SolutionPointer sol)
{
	uint64_t distance = 0;
	for (size_t i = 0; i < sol->size()-1; i++) {
		distance += (*adjacencyMatrix)[(*sol)[i]][(*sol)[i+1]];
	}
	distance += (*adjacencyMatrix)[(*sol)[sol->size()-1]][(*sol)[0]];
	return distance;
}
