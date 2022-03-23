#ifndef META_TSP_INSTANCE_H_
#define META_TSP_INSTANCE_H_

#include <vector>
#include <memory>
#include <string>
#include <random>
#include <fstream>
#include <iostream>

class TSPInstance;

using DistanceMatrix = std::vector<std::vector<uint32_t>>;
using DistanceMatrixPointer = std::shared_ptr<DistanceMatrix>;
using Solution = std::vector<uint32_t>;
using SolutionPointer = std::shared_ptr<Solution>;
using InstancePointer = std::shared_ptr<TSPInstance>;

class TSPInstance
{
public:
	TSPInstance(size_t size);
	~TSPInstance();

	DistanceMatrixPointer getDistanceMatrix();
	SolutionPointer getSolution();
	SolutionPointer getOptimalSolution();

	uint64_t calculateSolutionDistance();
	uint64_t calculateOptimalSolutionDistance();

	static InstancePointer generateAsymetricInstance(size_t size, unsigned int seed,  uint32_t minimumDistance = 0, uint32_t maximumDistance = 1000);
	static InstancePointer generateSymtericInstaance(size_t size, unsigned int seed, uint32_t minimumDistance = 0, uint32_t maximumDistance = 1000);
	static InstancePointer generateEuclidInstance(size_t size, unsigned int seed, uint32_t minimumDistance = 0, uint32_t maximumDistance = 1000);
	static InstancePointer loadFromFile(const std::string& filepath);

private:
	DistanceMatrixPointer adjacencyMatrix;
	SolutionPointer solution;
	SolutionPointer optimalSolution;

	uint64_t calculateGenericSolutionDistance(SolutionPointer solution);
};



#endif