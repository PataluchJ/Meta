#ifndef META_TSP_LOADER_H_
#define META_TSP_LOADER_H_

#include <Logger.hpp>

#include <fstream>
#include <vector>
#include <memory>
#include <math.h>

class TSPFile
{
public:
	TSPFile(const std::string& filepath);
	~TSPFile();
	
	using TSPinteger = int;
	using TSPreal = double;

	//bool openAndParse(const std::string& filename);
	bool isGood();

	std::string name;
	std::string comment;
	size_t dimension = 0;
	size_t capacity = 0;
	
	std::vector<std::vector<TSPinteger>> matrix;
	std::vector<TSPinteger> tour;

	enum class Type {
		TSP,
		ATSP,
		SOP,
		HCP,
		CVRP,
		TOUR,
		Unknow
	} type = Type::Unknow;
	enum class EdgeWeightType {
		Explicit,
		Euc2D,
		Euc3D,
		Max2D,
		Max3D,
		Man2D,
		Man3D,
		Ceil2D,
		Geo,
		ATT,
		XRAY1,
		XRAY2,
		Special,
		Unknow
	} weightType = EdgeWeightType::Unknow;
	enum class EdgeWeightFormat {
		Function,
		FullMatrix,
		UpperRow,
		LowerRow,
		UpperDiagRow,
		LowerDiagRow,
		UpperCol,
		LowerCol,
		UpperDiagCol,
		LowerDiagCol,
		Unknow
	} weightFormat = EdgeWeightFormat::Unknow;
	enum class EdgeDataFormat {
		EdgeList,
		AdjList,
		Unknow
	} dataFormat = EdgeDataFormat::Unknow;
	enum class NodeCoordType {
		TwodCoords,
		ThreedCoords,
		NoCoords,
		Unknow
	} coordType = NodeCoordType::NoCoords;
	enum class DisplayDataType {
		CoordDisplay,
		TwodDisplay,
		NoDisplay,
		Unknow
	} displayType = DisplayDataType::NoDisplay;

private:

	struct Pair {
		size_t x;
		size_t y;
		Pair(size_t x, size_t y) : x(x), y(y){}
	};
	struct Coord {
		TSPreal x;
		TSPreal y;
		TSPreal z;
		Coord() : x(0.0), y(0.0), z(0.0) {}
		Coord(TSPreal x, TSPreal y, TSPreal z) : x(x), y(y), z(z) {}
	};

	using IteratorForwardFunction = Pair(*)(const Pair, const size_t);
	using DistanceFunction = TSPinteger(*)(const Coord, const Coord);
	using VectorPointer = std::vector<Coord>;
	using CoordVectorPointer = std::unique_ptr<VectorPointer>;

	std::fstream file;
	bool success;

	enum class ParseMode {
		Specification,
		NodeCoord,
		DepotSection,
		DemandSection,
		EdgeData,
		FixedEdges,
		DisplayData,
		Tour,
		EdgeWeight,
		Eof
	} parseMode = ParseMode::Specification;

	void parseFile();
	void parseSpecificationLine(std::string& line);
	void parseDemandSection();
	void parseDepotSection();
	void parseDisplayDataSection();
	void parseEdgeDataSection();
	void parseEdgeWeightSection();
	void parseFixedEdgeSection();
	void parseNodeCoordSection();
	void parseTourSection();

	std::string sanitizeString(const std::string&& string);

	CoordVectorPointer load2DPoints();
	CoordVectorPointer load3DPoints();

	Type stringToType(const std::string& name);
	EdgeWeightType stringToWeightType(const std::string& name);
	EdgeWeightFormat stringToWeightFormat(const std::string& name);
	EdgeDataFormat stringToDataFormat(const std::string& name);
	NodeCoordType stringToCoordType(const std::string& name);
	DisplayDataType stringToDisplayType(const std::string& name);
};

#endif