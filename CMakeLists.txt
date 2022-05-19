cmake_minimum_required(VERSION 3.10)

project(Meta VERSION 1.3.0)
set(CMAKE_CXXX_STANDARD 17)
set(CMAKE_CXXX_STANDARD_REQUIRED True)

include_directories(Includes)

set(SRC_LIST
Sources/Greedy.cpp
Sources/KRandom.cpp
Sources/Logger.cpp
Sources/Solution.cpp
Sources/TabuSearch.cpp
Sources/TSPFile.cpp
Sources/TSPInstance.cpp
Sources/TwoOpt.cpp
Includes/Greedy.hpp
Includes/KRandom.hpp
Includes/Logger.hpp
Includes/NamedType.hpp
Includes/Solution.hpp
Includes/Solver.hpp
Includes/TabuSearch.hpp
Includes/TSPFile.hpp
Includes/TSPInstance.hpp
Includes/TwoOpt.hpp
)

add_executable(${PROJECT_NAME} Sources/main.cpp ${SRC_LIST})
