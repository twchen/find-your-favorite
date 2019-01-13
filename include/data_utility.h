#ifndef DATA_UTILITY_H
#define DATA_UTILITY_H


#include "data_struct.h"

point_t* alloc_point(int dim);

point_t* alloc_point(int dim, int id);

void release_point( point_t* &point_v);

point_t* rand_point(int dim);

point_set_t* alloc_point_set(int numberOfPoints);

void release_point_set(point_set_t* &point_set_v, bool clear);

hyperplane_t* alloc_hyperplane(point_t* normal, double offset);

hyperplane_set_t* alloc_hyperplane_set(int numberOfHyperplanes);

void release_hyperplane_set(hyperplane_set_t* &hyperplane_set_v, bool clear);

void release_hyperplane(hyperplane_t* &hyperplane_v);

void print_point(point_t* point_v);

void print_point_set(point_set_t* point_set_v);

#endif