#include "read_write.h"


/*
*	Read points from the input file according to the setting in the config file.
*/

point_set_t* read_points(char* input)
{
	FILE* c_fp;

	char filename[MAX_FILENAME_LENG];
	sprintf(filename, "input/%s", input);

	if ((c_fp = fopen(filename, "r")) == NULL)
	{
		fprintf(stderr, "Cannot open the data file %s.\n", filename);
		exit(0);
	}

	int number_of_points, dim;
	fscanf(c_fp, "%i%i", &number_of_points, &dim);

	point_set_t* point_set = alloc_point_set(number_of_points);

	// read points line by line
	for (int i = 0; i < number_of_points; i++)
	{
		point_t* p = alloc_point(dim, i);
		for (int j = 0; j < dim; j++)
		{
			fscanf(c_fp, "%lf", &p->coord[j]);
		}
		point_set->points[i] = p;
	}

	fclose(c_fp);
	return point_set;
}

point_set_t* process_car(point_set_t* P)
{
	//price	yearOfRegistration	powerPS	kilometer
	int dim = P->points[0]->dim;

	point_set_t* norm_P = alloc_point_set(P->numberOfPoints);

	for (int i = 0; i < norm_P->numberOfPoints; i++)
	{
		norm_P->points[i] = copy(P->points[i]);
		norm_P->points[i]->id = P->points[i]->id;
	}

	for (int j = 0; j < dim; j++)
	{
		double max = 0;
		double min = INF;

		for (int i = 0; i < norm_P->numberOfPoints; i++)
		{
			if (norm_P->points[i]->coord[j] > max)
				max = norm_P->points[i]->coord[j];
			if (norm_P->points[i]->coord[j] < min)
				min = norm_P->points[i]->coord[j];
		}

		for (int i = 0; i < norm_P->numberOfPoints; i++)
			norm_P->points[i]->coord[j] = (norm_P->points[i]->coord[j] - min) / (max - min);

		for (int i = 0; i < norm_P->numberOfPoints; i++)
			if (j == 0 || j == 3)
				norm_P->points[i]->coord[j] = 1 - norm_P->points[i]->coord[j];
	}


	return norm_P;
}


int dominates(point_t* p1, point_t* p2)
{
	int i;

	for (i = 0; i < p1->dim; ++i)
		if (p1->coord[i] < p2->coord[i])
			return 0;

	return 1;
}

point_set_t* skyline_point(point_set_t *p)
{
	int i, j, dominated, index = 0, m;
	point_t* pt;

	int* sl = new int[p->numberOfPoints];

	for (i = 0; i < p->numberOfPoints; ++i)
	{
		dominated = 0;
		pt = p->points[i];

		// check if pt is dominated by the skyline so far   
		for (j = 0; j < index && !dominated; ++j)
			if (dominates(p->points[sl[j]], pt))
				dominated = 1;

		if (!dominated)
		{
			// eliminate any points in current skyline that it dominates
			m = index;
			index = 0;
			for (j = 0; j < m; ++j)
				if (!dominates(pt, p->points[sl[j]]))
					sl[index++] = sl[j];

			// add this point as well
			sl[index++] = i;
		}
	}

	point_set_t* skyline = alloc_point_set(index);
	for (int i = 0; i < index; i++)
		skyline->points[i] = p->points[sl[i]];

	delete[] sl;
	return skyline;
}

void write_hyperplanes(vector<hyperplane_t*> utility_hyperplane, point_t* feasible_pt, char* filename)
{
	//char filename[MAX_FILENAME_LENG];
	int dim = feasible_pt->dim;

	FILE *wPtr = NULL;
	//sprintf(filename, "output/hyperplane_data");

	while(wPtr == NULL)
		wPtr = (FILE *)fopen(filename, "w");

	fprintf(wPtr, "%i\n1\n", dim);
	for(int i = 0; i < dim; i++)
		fprintf(wPtr, "%lf ", feasible_pt->coord[i]);
	fprintf(wPtr, "\n");

	fprintf(wPtr, "%i\n%i\n", dim+1, utility_hyperplane.size());
	for (int i = 0; i < utility_hyperplane.size(); i++)
	{
		for (int j = 0; j < dim; j++)
		{
			fprintf(wPtr, "%lf ",utility_hyperplane[i]->normal->coord[j]);
		}
		fprintf(wPtr, "%lf ",utility_hyperplane[i]->offset);
		fprintf(wPtr, "\n");
	}

	fclose(wPtr);

}
