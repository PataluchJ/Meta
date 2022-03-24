#include "TSPFile.h"

TSPFile::TSPFile(const std::string& filepath)
{
	this->file.open(filepath, std::ios::in);
	if(file.good())
		parseFile();
}

TSPFile::~TSPFile()
{
	file.close();
}

bool TSPFile::isGood()
{
	return this->file.good();
}

void TSPFile::parseFile()
{
	std::string line;
	while (std::getline(file, line) && parseMode != ParseMode::Eof) {
		parseSpecificationLine(line);
		switch (parseMode) {
			case ParseMode::DemandSection: {
				parseDemandSection();
			}break;
			case ParseMode::DepotSection: {
				parseDepotSection();
			}break;
			case ParseMode::DisplayData: {
				parseDisplayDataSection();
			}break;
			case ParseMode::EdgeData : {
				parseEdgeDataSection();
			}break;
			case ParseMode::EdgeWeight: {
				parseEdgeWeightSection();
			}break;
			case ParseMode::FixedEdges: {
				parseFixedEdgeSection();
			}break;
			case ParseMode::NodeCoord: {
				parseNodeCoordSection();
			}break;
			case ParseMode::Tour: {
				parseTourSection();
			}break;
		}
		parseMode = ParseMode::Specification;
	}
}

void TSPFile::parseDemandSection(){
	std::cerr << "DEMAND_SECTION not supported. Skiped.\n";
	/* Format:
	*	<integer - node number> <integer - demand>
	*	Skip this section
	*/
	std::string dumpLine;
	for (size_t i = 0; i < dimension; i++) {
		std::getline(file, dumpLine);
	}
}
void TSPFile::parseDepotSection() {
	std::cerr << "DEPOT_SECTION not supported.\n";
	/* Format:
	*	list terminated with -1
	*	Skip this section
	*/
	int readed;
	do {
		file >> readed;
	} while (readed != -1);
}
void TSPFile::parseDisplayDataSection() {
	std::cerr << "DISPLAY_DATA_SECTION not supported.\n";
}
void TSPFile::parseEdgeDataSection() {
	/* Format:
	* If EDGE_LIST then:
	*	<integer - v node> <integer - u node>
	*  Edge from v to u, list terminated with -1
	* If ADJ_LIST then:
	*	<integer - v> <intereg x1> ... <integer xn> -1
	*  Edges from v to x1/x2..., list terminated with -1
	* Skip this section
	*/
	int v, u;
	if (dataFormat == EdgeDataFormat::EdgeList) {
		bool endReached = false;
		while (!endReached) {
			file >> v;
			if (v != -1) {
				file >> u;
				// TODO: Save edge v->u
			}
			else
				endReached = true;
		}
	}
	else if(dataFormat == EdgeDataFormat::AdjList) {
		file >> v;
		while (v != -1) {
			file >> u;
			if (u == -1) {
				file >> v;
			}
			//else {
				//TODO: save v->u edged
			//}
		}
	}
}
void TSPFile::parseEdgeWeightSection() {
	if (weightType != EdgeWeightType::Explicit) {
		std::cerr << "EDGE_WEIGHT_SECTION is not explicit! Cannot parse.\n";
		return;
	}
	IteratorForwardFunction forward;
	Pair iterator(0, 0);
	bool triangular = true;
	bool diagonalIncluded = false;
	switch (weightFormat) {
	case EdgeWeightFormat::FullMatrix: {
		forward = [](const Pair i, const size_t size) {
			size_t x = i.x + 1;
			size_t y = i.y;
			if (x == size) {
				y += 1;
				x = 0;
			}
			return Pair(x, y);
		};
		triangular = false;
		diagonalIncluded = true;
	}break;
	case EdgeWeightFormat::UpperRow: {
		forward = [](const Pair i, const size_t size) {
			size_t x = i.x + 1;
			size_t y = i.y;
			if (x == size) {
				y += 1;
				x = y + 1;
			}
			return Pair(x, y);
		};
		iterator.x = 1;
	}break;
	case EdgeWeightFormat::LowerRow: {
		forward = [](const Pair i, const size_t size) {
			size_t x = i.x + 1;
			size_t y = i.y;
			if (x == y) {
				y += 1;
				x = 0;
			}
			return Pair(x, y);
		};
		iterator.y = 1;
	}break;
	case EdgeWeightFormat::UpperDiagRow: {
		forward = [](const Pair i, const size_t size) {
			size_t x = i.x + 1;
			size_t y = i.y;
			if (x == size) {
				y += 1;
				x = y;
			}
			return Pair(x, y);
		};
		diagonalIncluded = true;
	}break;
	case EdgeWeightFormat::LowerDiagRow: {
		forward = [](const Pair i, const size_t size) {
			size_t x = i.x + 1;
			size_t y = i.y;
			if (x > y) {
				y += 1;
				x = 0;
			}
			return Pair(x, y);
		};
		diagonalIncluded = true;
	}break;
	case EdgeWeightFormat::UpperCol: {
		forward = [](const Pair i, const size_t size) {
			size_t y = i.y + 1;
			size_t x = i.x;
			if (y == x) {
				x += 1;
				y = 0;
			}
			return Pair(x, y);
		};
		iterator.x = 1;
	}break;
	case EdgeWeightFormat::LowerCol: {
		forward = [](const Pair i, const size_t size) {
			size_t y = i.y + 1;
			size_t x = i.x;
			if (size == y) {
				x += 1;
				y = x + 1;
			}
			return Pair(x, y);
		};
		iterator.y = 1;
	}break;
	case EdgeWeightFormat::UpperDiagCol: {
		forward = [](const Pair i, const size_t size) {
			size_t y = i.y + 1;
			size_t x = i.x;
			if (y > x) {
				x += 1;
				y = 0;
			}
			return Pair(x, y);
		};
		diagonalIncluded = true;
	}break;
	case EdgeWeightFormat::LowerDiagCol: {
		forward = [](const Pair i, const size_t size) {
			size_t y = i.y + 1;
			size_t x = i.x;
			if (size == y) {
				x += 1;
				y = x;
			}
			return Pair(x, y);
		};
		diagonalIncluded = true;
	}break;
	default: {
		std::cerr << "Unknow Edge weight format.\n";
		return;
	} break;
	}

	size_t toLoad = dimension * dimension;
	if (triangular) {
		toLoad = (toLoad - dimension) / 2;
		if (diagonalIncluded)
			toLoad += dimension;
	}

	TSPinteger value;
	for (size_t loaded = 0; loaded < toLoad; loaded++) {
		file >> value;
		matrix[iterator.y][iterator.x] = value;
		if (triangular)
			matrix[iterator.x][iterator.y] = value;
		iterator = forward(iterator, dimension);
	}
}
void TSPFile::parseFixedEdgeSection() {
}
void TSPFile::parseNodeCoordSection() {
	DistanceFunction distance;
	bool twoDeee = true;
	switch (weightType) {
	case EdgeWeightType::Euc2D : {
		distance = [](const Coord a, const Coord b) {
			TSPreal xd = a.x - b.x;
			TSPreal yd = a.y - b.y;
			return (TSPinteger)(0.5+sqrt(xd * xd + yd * yd));
		};
	}break;
	case EdgeWeightType::Euc3D: {
		distance = [](const Coord a, const Coord b) {
			TSPreal xd = a.x - b.x;
			TSPreal yd = a.y - b.y;
			TSPreal zd = a.z - b.z;
			return (TSPinteger)(0.5 + sqrt(xd * xd + yd * yd + zd*zd));
		};
		twoDeee = false;
	}break;
	case EdgeWeightType::Man2D: {
		distance = [](const Coord a, const Coord b) {
			TSPreal xd = abs(a.x - b.x);
			TSPreal yd = abs(a.y - b.y);
			return (TSPinteger)(0.5 + xd + yd);
		};
	}break;
	case EdgeWeightType::Man3D: {
		distance = [](const Coord a, const Coord b) {
			TSPreal xd = abs(a.x - b.x);
			TSPreal yd = abs(a.y - b.y);
			TSPreal zd = abs(a.z - b.z);
			return (TSPinteger)(0.5 + xd+yd+zd);
		};
		twoDeee = false;
	}break;
	case EdgeWeightType::Max2D: {
		distance = [](const Coord a, const Coord b) {
			TSPinteger xd = abs(a.x - b.x) + 0.5;
			TSPinteger yd = abs(a.y - b.y) + 0.5;
			return xd > yd ? xd : yd;
		};
	}break;
	case EdgeWeightType::Max3D: {
		distance = [](const Coord a, const Coord b) {
			TSPinteger xd = abs(a.x - b.x) + 0.5;
			TSPinteger yd = abs(a.y - b.y) + 0.5;
			TSPinteger zd = abs(a.z - b.z) + 0.5;
			return xd > yd ? ( xd > zd ? xd : zd) : (yd > zd ? yd : zd);
		};
		twoDeee = false;
	}break;
	case EdgeWeightType::Geo: {
		distance = [](const Coord a, const Coord b) {
			constexpr TSPreal PI = 3.141592;
			constexpr TSPreal RRR = 6378.388;
			TSPreal initialValues[] = {a.x, a.y, b.x, b.y};
			TSPreal converted[] = { 0.0,0.0,0.0,0.0 };
			for (int i = 0; i < 4; i++) {
				TSPinteger deg = initialValues[i] + 0.5;
				TSPreal min = initialValues[i] - deg;
				converted[i] = PI * (deg + 5.0 * min / 3.0) / 180.0;
			}
			TSPreal q[] = {
				cos(converted[1] - converted[3]),
				cos(converted[0] - converted[2]),
				cos(converted[0] + converted[2])
			};
			return (TSPinteger)(RRR * acos(0.5 * ((1.0 + q[0]) * q[1] - (1.0 - q[0]) * q[2])) + 1.0);
		};
	}break;
	case EdgeWeightType::ATT:{
		distance = [](const Coord a, const Coord b) {
			TSPinteger xd = (a.x - b.x);
			TSPinteger yd = (a.y - b.y);
			TSPreal rij = sqrt((xd * xd + yd * yd) / 10.0);
			TSPinteger tij = (TSPinteger)(rij + 0.5);
			if (tij < rij)
				return tij + 1;
			return tij;
		};
	}break;
	case EdgeWeightType::Ceil2D: {
		distance = [](const Coord a, const Coord b) {
			TSPreal xd = a.x - b.x;
			TSPreal yd = a.y - b.y;
			return (TSPinteger)(1.0 + sqrt(xd * xd + yd * yd));
		};
	}break;
	default: {
		std::cerr << "Unsupported distance function.\n";
		return;
	}break;
	}
	CoordVectorPointer coords;
	if (twoDeee)
		coords = load2DPoints();
	else
		coords = load3DPoints();

	for (size_t v = 0; v < dimension; v++) {
		for (size_t u = v + 1; u < dimension; u++) {
			auto d = distance((*coords)[v], (*coords)[u]);
			matrix[v][u] = d;
			matrix[u][v] = d;
		}
	}
}
void TSPFile::parseTourSection() {
	tour.reserve(dimension+1);
	TSPinteger readed;
	do {
		file >> readed;
		tour.push_back(readed);
	} while (readed != -1);
	tour.pop_back(); // Pop -1
}

TSPFile::CoordVectorPointer TSPFile::load2DPoints()
{
	CoordVectorPointer coords = CoordVectorPointer(new VectorPointer);
	coords->resize(dimension);
	TSPinteger v;
	for (size_t i = 0; i < dimension; i++) {
		file >> v;
		file >> (*coords)[v].x >> (*coords)[v].y;
	}
	return coords;
}
TSPFile::CoordVectorPointer TSPFile::load3DPoints()
{
	CoordVectorPointer coords = CoordVectorPointer(new VectorPointer);
	coords->resize(dimension);
	TSPinteger v;
	for (size_t i = 0; i < dimension; i++) {
		file >> v;
		file >> (*coords)[v].x >> (*coords)[v].y >> (*coords)[v].z;
	}
	return coords;
}

void TSPFile::parseSpecificationLine(std::string& line)
{
	auto colonPosition = line.find(':');
	if (colonPosition != std::string::npos) {
		/* key: value */
		std::string key = line.substr(0, colonPosition);
		std::string value = line.substr(colonPosition + 2, line.size() - colonPosition - 2);
		if (key.compare("NAME") == 0)
			this->name = value;
		else if (key.compare("COMMENT") == 0)
			this->comment = value;
		else if (key.compare("DIMENSION") == 0) {
			this->dimension = std::stoi(value);
			matrix.resize(dimension);
			for (auto& m : matrix)
				m.resize(dimension, 0);
		}
		else if (key.compare("TYPE") == 0)
			this->type = stringToType(value);
		else if (key.compare("CAPACITY") == 0)
			this->capacity = std::stoi(value);
		else if (key.compare("EDGE_WEIGHT_TYPE") == 0)
			this->weightType = stringToWeightType(value);
		else if (key.compare("EDGE_WEIGHT_FORMAT") == 0)
			this->weightFormat = stringToWeightFormat(value);
		else if (key.compare("NODE_COORD_TYPE") == 0)
			this->coordType = stringToCoordType(value);
		else if (key.compare("DISPLAY_DATA_TYPE") == 0)
			this->displayType = stringToDisplayType(value);
		else
			std::cerr << "Unknow specyfication key: " << key << " : " << value << "\n";
	}
	else {
		/* section */
		if (line.compare("NODE_COORD_SECTION") == 0) {
			this->parseMode = ParseMode::NodeCoord;
		}
		else if (line.compare("DEPOT_SECTION") == 0) {
			this->parseMode = ParseMode::DepotSection;
		}
		else if (line.compare("DEMAND_SECTION") == 0) {
			this->parseMode = ParseMode::DemandSection;
		}
		else if (line.compare("EDGE_DATA_SECTION") == 0) {
			this->parseMode = ParseMode::EdgeData;
		}
		else if (line.compare("FIXED_EDGE_SECTION") == 0) {
			//std::cerr << "FIXED_EDGE_SECTION not supported.\n";
			this->parseMode = ParseMode::FixedEdges;
		}
		else if (line.compare("DISPLAY_DATA_SECTION") == 0) {
			//std::cerr << "DISPLAY_DATA_SECTION not supported.\n";
			this->parseMode = ParseMode::DisplayData;
		}
		else if (line.compare("TOUR_SECTION") == 0) {
			//std::cerr << "TOUR_SECTION not supported.\n";
			this->parseMode = ParseMode::Tour;
		}
		else if (line.compare("EDGE_WEIGHT_SECTION") == 0) {
			//std::cerr << "EDGE_WEIGHT_SECTION not supported.\n";
			this->parseMode = ParseMode::EdgeWeight;
		}
		else if (line.compare("EOF") == 0) {
			this->parseMode = ParseMode::Eof;
		}
		else {
			std::cerr << "Unknow section: " << line << "\n";
		}

	}
}


TSPFile::Type TSPFile::stringToType(const std::string& name) {
	if (name.compare("TSP") == 0)
		return Type::TSP;
	if (name.compare("ATSP") == 0)
		return Type::ATSP;
	if (name.compare("SOP") == 0)
		return Type::SOP;
	if (name.compare("HCP") == 0)
		return Type::HCP;
	if (name.compare("CVRP") == 0)
		return Type::CVRP;
	if (name.compare("TOUR") == 0)
		return Type::TOUR;
	std::cerr << "Unknow problem type: " << name << "\n";
	return Type::Unknow;
}
TSPFile::EdgeWeightType TSPFile::stringToWeightType(const std::string& name) {
	if (name.compare("EXPLICIT") == 0)
		return EdgeWeightType::Explicit;
	if (name.compare("EUC_2D") == 0)
		return EdgeWeightType::Euc2D;
	if (name.compare("EUC_3D") == 0)
		return EdgeWeightType::Euc3D;
	if (name.compare("MAX_2D") == 0)
		return EdgeWeightType::Max2D;
	if (name.compare("MAX_3D") == 0)
		return EdgeWeightType::Max3D;
	if (name.compare("MAN_2D") == 0)
		return EdgeWeightType::Man2D;
	if (name.compare("MAN_3D") == 0)
		return EdgeWeightType::Man3D;
	if (name.compare("CEIL_2D") == 0)
		return EdgeWeightType::Ceil2D;
	if (name.compare("GEO") == 0)
		return EdgeWeightType::Geo;
	if (name.compare("ATT") == 0)
		return EdgeWeightType::ATT;
	if (name.compare("XRAY1") == 0)
		return EdgeWeightType::XRAY1;
	if (name.compare("XRAY2") == 0)
		return EdgeWeightType::XRAY2;
	if (name.compare("SPECIAL") == 0)
		return EdgeWeightType::Special;
	std::cerr << "Unknow weight type: " << name << "\n";
	return EdgeWeightType::Unknow;
}
TSPFile::EdgeWeightFormat TSPFile::stringToWeightFormat(const std::string& name) {
	if (name.compare("FUNCTION") == 0)
		return EdgeWeightFormat::Function;
	if (name.compare("FULL_MATRIX") == 0)
		return EdgeWeightFormat::FullMatrix;
	if (name.compare("UPPER_ROW") == 0)
		return EdgeWeightFormat::UpperRow;
	if (name.compare("LOWER_ROW") == 0)
		return EdgeWeightFormat::LowerRow;
	if (name.compare("UPPER_DIAG_ROW") == 0)
		return EdgeWeightFormat::UpperDiagRow;
	if (name.compare("LOWER_DIAG_ROW ") == 0)
		return EdgeWeightFormat::LowerDiagRow;
	if (name.compare("UPPER_COL") == 0)
		return EdgeWeightFormat::UpperCol;
	if (name.compare("LOWER_COL") == 0)
		return EdgeWeightFormat::LowerCol;
	if (name.compare("UPPER_DIAG_COL") == 0)
		return EdgeWeightFormat::UpperDiagCol;
	if (name.compare("LOWER_DIAG_COL") == 0)
		return EdgeWeightFormat::LowerDiagCol;
	std::cerr << "Unknow weight format: " << name << "\n";
	return EdgeWeightFormat::Unknow;
}
TSPFile::EdgeDataFormat TSPFile::stringToDataFormat(const std::string& name) {
	if (name.compare("ADJ_LIST") == 0)
		return EdgeDataFormat::AdjList;
	if (name.compare("EDGE_LIST") == 0)
		return EdgeDataFormat::EdgeList;
	std::cerr << "Unknow data format: " << name << "\n";
	return EdgeDataFormat::Unknow;
}
TSPFile::NodeCoordType TSPFile::stringToCoordType(const std::string& name) {
	if (name.compare("TWOD_COORDS") == 0)
		return NodeCoordType::TwodCoords;
	if (name.compare("THREED_COORDS") == 0)
		return NodeCoordType::ThreedCoords;
	if (name.compare("NO_CORDS") == 0)
		return NodeCoordType::NoCoords;
	std::cerr << "Unknow coord type: " << name << "\n";
	return NodeCoordType::Unknow;
}
TSPFile::DisplayDataType TSPFile::stringToDisplayType(const std::string& name) {
	if (name.compare("COORD_DISPLAY") == 0)
		return DisplayDataType::CoordDisplay;
	if (name.compare("TWOD_DISPLAY") == 0)
		return DisplayDataType::TwodDisplay;
	if (name.compare("NO_DISPLAY") == 0)
		return DisplayDataType::NoDisplay;
	std::cerr << "Unknow display type: " << name << "\n";
	return DisplayDataType::Unknow;
}
