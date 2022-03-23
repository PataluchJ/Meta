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
	std::fstream file;
	file.open(filepath, std::ios::in);
	
	enum class Mode {
		Keywords,
		Data
	} mode = Mode::Keywords;
	enum class Type {
		Unknow,
		TSP,
		ATSP,
		TOUR,
		Other
	} type = Type::Unknow;

	std::string line;
	std::string key;
	std::string value;

	while (std::getline(file, line)) {
		if (mode == Mode::Keywords) {
			auto colonPosition = line.find(':');
			if (colonPosition != std::string::npos) {
				key = line.substr(0, colonPosition);
				value = line.substr(colonPosition+2, line.size() - colonPosition-2);
			}
			else {
				key = line;
			}
			if (key.compare("TYPE") == 0) {

			}
		}
	}

	file.close();
	return InstancePointer();
}

uint64_t TSPInstance::calculateGenericSolutionDistance(SolutionPointer solution)
{
	uint64_t distance = 0;
	for (size_t i = 0; i < solution->size()-1; i++) {
		distance += (*adjacencyMatrix)[(*solution)[i]][(*solution)[i+1]];
	}
	distance += (*adjacencyMatrix)[(*solution)[solution->size()-1]][(*solution)[0]];
	return distance;
}
