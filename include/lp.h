#ifndef LP_H
#define LP_H

#include <glpk.h>
#include "data_struct.h"
#include "data_utility.h"
#include "operation.h"
#include <vector>

bool insideCone(std::vector<point_t*> ExRays, point_t* pt);

point_t* find_feasible(std::vector<hyperplane_t*> hyperplane);

void solveLP(std::vector<point_t*> B, point_t* b, double& theta, point_t* & pi);

#endif