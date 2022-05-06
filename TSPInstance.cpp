#include "TSPInstance.h"

TSPInstance::TSPInstance(size_t size)
{
	/* Set pointers */
	this->adjacencyMatrix = DistanceMatrixPointer(new DistanceMatrix);
	/* Resize containers */
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

InstancePointer TSPInstance::generateAsymetricInstance(size_t size, unsigned int seed, uint32_t minimumDistance, uint32_t maximumDistance)
{
	if (minimumDistance > maximumDistance) {
		std::cerr << "Minimal distance must be smaller than maximum.\n";
		return nullptr;
	}
	InstancePointer instance = InstancePointer(new TSPInstance(size));
	auto matrix = instance->getDistanceMatrix();
	auto modulo = maximumDistance - minimumDistance;
	srand(seed);
	for (size_t v = 0; v < size; v++) 
		for (size_t u = 0; u < size; u++) 
			(*matrix)[v][u] = rand() % modulo + minimumDistance;

	for (size_t v = 0; v < size; v++)
		(*matrix)[v][v] = 0;

	return instance;
}

InstancePointer TSPInstance::generateSymtericInstaance(size_t size, unsigned int seed, uint32_t minimumDistance, uint32_t maximumDistance)
{
	if (minimumDistance > maximumDistance) {
		std::cerr << "Minimal distance must be smaller than maximum.\n";
		return nullptr;
	}
	InstancePointer instance = InstancePointer(new TSPInstance(size));
	auto matrix = instance->getDistanceMatrix();
	auto modulo = maximumDistance - minimumDistance;
	srand(seed);
	for (size_t v = 0; v < size; v++) {
		for (size_t u = v + 1; u < size; u++) {
			uint32_t distance = rand() % modulo + minimumDistance;
			(*matrix)[v][u] = distance;
			(*matrix)[u][v] = distance;
		}
	}

	return instance;
}

InstancePointer TSPInstance::generateEuclidInstance(size_t size, unsigned int seed, uint32_t distance )
{
	std::vector<float> xCoord;
	std::vector<float> yCoord;
	xCoord.resize(size, 0);
	yCoord.resize(size, 0);
	
	/* Generate points */
	double divider = static_cast<double>(RAND_MAX / (distance));
	for (size_t i = 0; i < size; i++) {
		xCoord[i] = static_cast<double>(rand() / divider);
		yCoord[i] = static_cast<double>(rand() / divider);
	}
	
	/* Create distance matrix */
	InstancePointer instance = InstancePointer(new TSPInstance(size));
	auto matrix = instance->getDistanceMatrix();
	for (size_t v = 0; v < size; v++) {
		for (size_t u = v+1; u < size; u++) {
			uint32_t distance = (uint32_t)(sqrt((xCoord[v] - xCoord[u]) * (xCoord[v] - xCoord[u]) +
				(yCoord[v] - yCoord[u]) * (yCoord[v] - yCoord[u])) + 0.5);
			
			(*matrix)[v][u] = distance;
			(*matrix)[u][v] = distance;
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

	for (size_t i = 0; i < file.dimension; i++) {
		for (size_t j = 0; j < file.dimension; j++) {
			(*instance->adjacencyMatrix)[i][j] = (uint32_t)(file.matrix[i][j]);
		}
	}

	return instance;
}

Solution TSPInstance::loadTourFromFile(const std::string& filepath)
{
	TSPFile file(filepath);
	if (!file.isGood()) {
		std::cerr << "Unable to load instance from file.\n";
		return Solution{};
	}
	Solution optimalSolution;
	optimalSolution.resize(file.dimension, 0);
	if (file.tour.size() != file.dimension) {
		std::cerr << "File does not contain tour section!";
		return Solution{};
	}

	for (size_t i = 0; i < file.dimension; i++) {
		optimalSolution[i] = file.tour[i];
	}

	return optimalSolution;
}

uint64_t TSPInstance::calculateGenericSolutionDistance(Solution& solution)
{
	uint64_t distance = 0;
	for (size_t i = 0; i < solution.size()-1; i++) {
		distance += (*adjacencyMatrix)[solution[i]][solution[i+1]];
	}
	distance += (*adjacencyMatrix)[solution[solution.size()-1]][solution[0]];
	return distance;
}
