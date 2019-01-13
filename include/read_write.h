#ifndef READ_WRITE_H
#define READ_WRITE_H

#include "data_struct.h"
#include "data_utility.h"
#include "operation.h"


point_set_t* read_points(char* input);

point_set_t* process_car(point_set_t* P);

int dominates(point_t* p1, point_t* p2);

point_set_t* skyline_point(point_set_t *p);

void write_hyperplanes(vector<hyperplane_t*> utility_hyperplane, point_t* feasible_pt, char* filename);


#endif