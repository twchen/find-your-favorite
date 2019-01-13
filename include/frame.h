#ifndef FRAME_H
#define FRAME_H

#include "data_struct.h"
#include "data_utility.h"
#include "lp.h"
#include "operation.h"

void frameConeFastLP(vector<point_t*> rays, vector<int>& idxs);

void partialConeFastLP(vector<point_t*> rays, vector<int>& B, int s);

void frameConeLP(std::vector<point_t*> rays, std::vector<int>& idxs);

#endif