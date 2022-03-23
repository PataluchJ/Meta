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
		switch (parseMode) {
			case ParseMode::Specification: {
				parseSpecificationLine(line);
			}break;
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
	}
}

void TSPFile::parseDemandSection(){
}
void TSPFile::parseDepotSection() {
}
void TSPFile::parseDisplayDataSection() {
}
void TSPFile::parseEdgeDataSection() {
}
void TSPFile::parseEdgeWeightSection() {
}
void TSPFile::parseFixedEdgeSection() {
}
void TSPFile::parseNodeCoordSection() {
}
void TSPFile::parseTourSection() {
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
		else if (key.compare("DIMENSION") == 0)
			this->dimension = std::stoi(value);
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
			//std::cerr << "NODE_COORD_SECTION not supported.\n";
			this->parseMode = ParseMode::NodeCoord;
		}
		else if (line.compare("DEPOT_SECTION") == 0) {
			//std::cerr << "DEPOT_SECTION not supported.\n";
			this->parseMode = ParseMode::DepotSection;
		}
		else if (line.compare("DEMAND_SECTION") == 0) {
			//std::cerr << "DEMAND_SECTION not supported.\n";
			this->parseMode = ParseMode::DemandSection;
		}
		else if (line.compare("EDGE_DATA_SECTION") == 0) {
			//std::cerr << "EDGE_DATA_SECTION not supported.\n";
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
	if (name.compare("TSP"))
		return Type::TSP;
	if (name.compare("ATSP"))
		return Type::ATSP;
	if (name.compare("SOP"))
		return Type::SOP;
	if (name.compare("HCP"))
		return Type::HCP;
	if (name.compare("CVRP"))
		return Type::CVRP;
	if (name.compare("TOUR"))
		return Type::TOUR;
	std::cerr << "Unknow problem type: " << name << "\n";
	return Type::Unknow;
}
TSPFile::EdgeWeightType TSPFile::stringToWeightType(const std::string& name) {
	if (name.compare("EXPLICIT"))
		return EdgeWeightType::Explicit;
	if (name.compare("EUC_2D"))
		return EdgeWeightType::Euc2D;
	if (name.compare("EUC_3D"))
		return EdgeWeightType::Euc3D;
	if (name.compare("MAX_2D"))
		return EdgeWeightType::Max2D;
	if (name.compare("MAX_3D"))
		return EdgeWeightType::Max3D;
	if (name.compare("MAN_2D"))
		return EdgeWeightType::Man2D;
	if (name.compare("MAN_3D"))
		return EdgeWeightType::Man3D;
	if (name.compare("CEIL_2D"))
		return EdgeWeightType::Ceil2D;
	if (name.compare("GEO"))
		return EdgeWeightType::Geo;
	if (name.compare("ATT"))
		return EdgeWeightType::ATT;
	if (name.compare("XRAY1"))
		return EdgeWeightType::XRAY1;
	if (name.compare("XRAY2"))
		return EdgeWeightType::XRAY2;
	if (name.compare("SPECIAL"))
		return EdgeWeightType::Special;
	std::cerr << "Unknow weight type: " << name << "\n";
	return EdgeWeightType::Unknow;
}
TSPFile::EdgeWeightFormat TSPFile::stringToWeightFormat(const std::string& name) {
	if (name.compare("FUNCTION"))
		return EdgeWeightFormat::Function;
	if (name.compare("FULL_MATRIX"))
		return EdgeWeightFormat::FullMatrix;
	if (name.compare("UPPER_ROW"))
		return EdgeWeightFormat::UpperRow;
	if (name.compare("LOWER_ROW"))
		return EdgeWeightFormat::LowerRow;
	if (name.compare("UPPER_DIAG_ROW"))
		return EdgeWeightFormat::UpperDiagRow;
	if (name.compare("LOWER_DIAG_ROW"))
		return EdgeWeightFormat::LowerDiagRow;
	if (name.compare("UPPER_COL"))
		return EdgeWeightFormat::UpperCol;
	if (name.compare("LOWER_COL"))
		return EdgeWeightFormat::LowerCol;
	if (name.compare("UPPER_DIAG_COL"))
		return EdgeWeightFormat::UpperDiagCol;
	if (name.compare("LOWER_DIAG_COL"))
		return EdgeWeightFormat::LowerDiagCol;
	std::cerr << "Unknow weight format: " << name << "\n";
	return EdgeWeightFormat::Unknow;
}
TSPFile::EdgeDataFormat TSPFile::stringToDataFormat(const std::string& name) {
	if (name.compare("ADJ_LIST"))
		return EdgeDataFormat::AdjList;
	if (name.compare("EDGE_LIST"))
		return EdgeDataFormat::EdgeList;
	std::cerr << "Unknow data format: " << name << "\n";
	return EdgeDataFormat::Unknow;
}
TSPFile::NodeCoordType TSPFile::stringToCoordType(const std::string& name) {
	if (name.compare("TWOD_COORDS"))
		return NodeCoordType::TwodCoords;
	if (name.compare("THREED_COORDS"))
		return NodeCoordType::ThreedCoords;
	if (name.compare("NO_CORDS"))
		return NodeCoordType::NoCoords;
	std::cerr << "Unknow coord type: " << name << "\n";
	return NodeCoordType::Unknow;
}
TSPFile::DisplayDataType TSPFile::stringToDisplayType(const std::string& name) {
	if (name.compare("COORD_DISPLAY"))
		return DisplayDataType::CoordDisplay;
	if (name.compare("TWOD_DISPLAY"))
		return DisplayDataType::TwodDisplay;
	if (name.compare("NO_DISPLAY"))
		return DisplayDataType::NoDisplay;
	std::cerr << "Unknow display type: " << name << "\n";
	return DisplayDataType::Unknow;
}
