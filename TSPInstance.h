#ifndef META_TSP_INSTANCE_H_
#define META_TSP_INSTANCE_H_

#include <vector>
#include <memory>
#include <string>
#include <random>
#include <fstream>
#include <iostream>

#include "Solution.h"
#include "TSPFile.h"

class TSPInstance;

using DistanceMatrix = std::vector<std::vector<uint32_t>>;
using DistanceMatrixPointer = std::shared_ptr<DistanceMatrix>;
using InstancePointer = std::shared_ptr<TSPInstance>;

class TSPInstance
{
public:
	TSPInstance(size_t size);
	~TSPInstance();

	DistanceMatrixPointer getDistanceMatrix();
	uint64_t calculateGenericSolutionDistance(Solution& solution);

	static InstancePointer generateAsymetricInstance(size_t size, unsigned int seed,  uint32_t minimumDistance = 0, uint32_t maximumDistance = 1000);
	static InstancePointer generateSymtericInstaance(size_t size, unsigned int seed, uint32_t minimumDistance = 0, uint32_t maximumDistance = 1000);
	static InstancePointer generateEuclidInstance(size_t size, unsigned int seed, uint32_t distance = 100);
	static InstancePointer loadFromFile(const std::string& filepath);
	static Solution loadTourFromFile(const std::string& filepath);

private:
	DistanceMatrixPointer adjacencyMatrix;
};



#endif
