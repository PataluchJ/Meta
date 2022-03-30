#ifndef META_TSP_TIMETESTER_H_
#define META_TSP_TIMETESTER_H_

#include "TSPInstance.h"
#include "Solver.h"

class TimeTester
{
public:
    void test(Solver *, int, int, std::string);
};

#endif
