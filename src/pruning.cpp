#include "pruning.h"

char hidden_options[]=" d n v Qbb QbB Qf Qg Qm Qr QR Qv Qx Qz TR E V Fa FA FC FD FS Ft FV Gt Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7 Q8 Q9 ";

#ifdef WIN32
#ifdef __cplusplus 
	extern "C" { 
#endif 
#endif

//#include "data_utility.h"

#include "mem.h"
#include "qset.h"
#include "libqhull.h"
#include "qhull_a.h"

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#if __MWERKS__ && __POWERPC__
#include <SIOUX.h>
#include <Files.h>
#include <console.h>
#include <Desk.h>

#elif __cplusplus
extern "C" {
  int isatty(int);
}

#elif _MSC_VER
#include <io.h>
#define isatty _isatty
int _isatty(int);

#else
int isatty(int);  /* returns 1 if stdin is a tty
                   if "Undefined symbol" this can be deleted along with call in main() */
#endif

#ifdef WIN32
#ifdef __cplusplus 
	}  
#endif
#endif

char qh_prompta[]= "\n\
qhalf- compute the intersection of halfspaces about a point\n\
    http://www.qhull.org  %s\n\
\n\
input (stdin):\n\
    optional interior point: dimension, 1, coordinates\n\
    first lines: dimension+1 and number of halfspaces\n\
    other lines: halfspace coefficients followed by offset\n\
    comments:    start with a non-numeric character\n\
\n\
options:\n\
    Hn,n - specify coordinates of interior point\n\
    Qt   - triangulated output\n\
    QJ   - joggled input instead of merged facets\n\
    Qc   - keep coplanar halfspaces\n\
    Qi   - keep other redundant halfspaces\n\
\n\
Qhull control options:\n\
    QJn  - randomly joggle input in range [-n,n]\n\
%s%s%s%s";  /* split up qh_prompt for Visual C++ */
char qh_promptb[]= "\
    Qbk:0Bk:0 - remove k-th coordinate from input\n\
    Qs   - search all halfspaces for the initial simplex\n\
    QGn  - print intersection if visible to halfspace n, -n for not\n\
    QVn  - print intersections for halfspace n, -n if not\n\
\n\
";
char qh_promptc[]= "\
Trace options:\n\
    T4   - trace at level n, 4=all, 5=mem/gauss, -1= events\n\
    Tc   - check frequently during execution\n\
    Ts   - print statistics\n\
    Tv   - verify result: structure, convexity, and redundancy\n\
    Tz   - send all output to stdout\n\
    TFn  - report summary when n or more facets created\n\
    TI file - input data from file, no spaces or single quotes\n\
    TO file - output results to file, may be enclosed in single quotes\n\
    TPn  - turn on tracing when halfspace n added to intersection\n\
    TMn  - turn on tracing at merge n\n\
    TWn  - trace merge facets when width > n\n\
    TVn  - stop qhull after adding halfspace n, -n for before (see TCn)\n\
    TCn  - stop qhull after building cone for halfspace n (see TVn)\n\
\n\
Precision options:\n\
    Cn   - radius of centrum (roundoff added).  Merge facets if non-convex\n\
     An  - cosine of maximum angle.  Merge facets if cosine > n or non-convex\n\
           C-0 roundoff, A-0.99/C-0.01 pre-merge, A0.99/C0.01 post-merge\n\
    Rn   - randomly perturb computations by a factor of [1-n,1+n]\n\
    Un   - max distance below plane for a new, coplanar halfspace\n\
    Wn   - min facet width for outside halfspace (before roundoff)\n\
\n\
Output formats (may be combined; if none, produces a summary to stdout):\n\
    f    - facet dump\n\
    G    - Geomview output (dual convex hull)\n\
    i    - non-redundant halfspaces incident to each intersection\n\
    m    - Mathematica output (dual convex hull)\n\
    o    - OFF format (dual convex hull: dimension, points, and facets)\n\
    p    - vertex coordinates of dual convex hull (coplanars if 'Qc' or 'Qi')\n\
    s    - summary (stderr)\n\
\n\
";
char qh_promptd[]= "\
More formats:\n\
    Fc   - count plus redundant halfspaces for each intersection\n\
         -   Qc (default) for coplanar and Qi for other redundant\n\
    Fd   - use cdd format for input (homogeneous with offset first)\n\
    FF   - facet dump without ridges\n\
    FI   - ID of each intersection\n\
    Fm   - merge count for each intersection (511 max)\n\
    FM   - Maple output (dual convex hull)\n\
    Fn   - count plus neighboring intersections for each intersection\n\
    FN   - count plus intersections for each non-redundant halfspace\n\
    FO   - options and precision constants\n\
    Fp   - dim, count, and intersection coordinates\n\
    FP   - nearest halfspace and distance for each redundant halfspace\n\
    FQ   - command used for qhalf\n\
    Fs   - summary: #int (8), dim, #halfspaces, #non-redundant, #intersections\n\
                      for output: #non-redundant, #intersections, #coplanar\n\
                                  halfspaces, #non-simplicial intersections\n\
                    #real (2), max outer plane, min vertex\n\
    Fv   - count plus non-redundant halfspaces for each intersection\n\
    Fx   - non-redundant halfspaces\n\
\n\
";
char qh_prompte[]= "\
Geomview output (2-d, 3-d and 4-d; dual convex hull)\n\
    Ga   - all points (i.e., transformed halfspaces) as dots\n\
     Gp  -  coplanar points and vertices as radii\n\
     Gv  -  vertices (i.e., non-redundant halfspaces) as spheres\n\
    Gi   - inner planes (i.e., halfspace intersections) only\n\
     Gn  -  no planes\n\
     Go  -  outer planes only\n\
    Gc   - centrums\n\
    Gh   - hyperplane intersections\n\
    Gr   - ridges\n\
    GDn  - drop dimension n in 3-d and 4-d output\n\
\n\
Print options:\n\
    PAn  - keep n largest facets (i.e., intersections) by area\n\
    Pdk:n- drop facet if normal[k] <= n (default 0.0)\n\
    PDk:n- drop facet if normal[k] >= n\n\
    Pg   - print good facets (needs 'QGn' or 'QVn')\n\
    PFn  - keep facets whose area is at least n\n\
    PG   - print neighbors of good facets\n\
    PMn  - keep n facets with most merges\n\
    Po   - force output.  If error, output neighborhood of facet\n\
    Pp   - do not report precision problems\n\
\n\
    .    - list of all options\n\
    -    - one line descriptions of all options\n\
    -V   - version\n\
";

char qh_prompt2[]= "\n\
qhalf- halfspace intersection about a point.  Qhull %s\n\
    input (stdin): [dim, 1, interior point], dim+1, n, coefficients+offset\n\
    comments start with a non-numeric character\n\
\n\
options (qhalf.htm):\n\
    Hn,n - specify coordinates of interior point\n\
    Qt   - triangulated output\n\
    QJ   - joggled input instead of merged facets\n\
    Tv   - verify result: structure, convexity, and redundancy\n\
    .    - concise list of all options\n\
    -    - one-line description of all options\n\
    -V   - version\n\
\n\
output options (subset):\n\
    s    - summary of results (default)\n\
    Fp   - intersection coordinates\n\
    Fv   - non-redundant halfspaces incident to each intersection\n\
    Fx   - non-redundant halfspaces\n\
    o    - OFF file format (dual convex hull)\n\
    G    - Geomview output (dual convex hull)\n\
    m    - Mathematica output (dual convex hull)\n\
    QVn  - print intersections for halfspace n, -n if not\n\
    TO file - output results to file, may be enclosed in single quotes\n\
\n\
examples:\n\
    rbox d | qconvex FQ n | qhalf s H0,0,0 Fp\n\
    rbox c | qconvex FQ FV n | qhalf s i\n\
    rbox c | qconvex FQ FV n | qhalf s o\n\
\n\
";
/* for opts, don't assign 'e' or 'E' to a flag (already used for exponent) */

/*-<a                             href="../libqhull/qh-qhull.htm#TOC"
  >-------------------------------</a><a name="prompt3">-</a>

  qh_prompt3
    concise prompt for qhull
*/
char qh_prompt3[]= "\n\
Qhull %s.\n\
Except for 'F.' and 'PG', upper_case options take an argument.\n\
\n\
 incidences     Geomview       mathematica    OFF_format     point_dual\n\
 summary        facet_dump\n\
\n\
 Fc_redundant   Fd_cdd_in      FF_dump_xridge FIDs           Fmerges\n\
 Fneighbors     FN_intersect   FOptions       Fp_coordinates FP_nearest\n\
 FQhalf         Fsummary       Fv_halfspace   FMaple         Fx_non_redundant\n\
\n\
 Gvertices      Gpoints        Gall_points    Gno_planes     Ginner\n\
 Gcentrums      Ghyperplanes   Gridges        Gouter         GDrop_dim\n\
\n\
 PArea_keep     Pdrop d0:0D0   Pgood          PFacet_area_keep\n\
 PGood_neighbors PMerge_keep   Poutput_forced Pprecision_not\n\
\n\
 Qbk:0Bk:0_drop Qcoplanar      QG_half_good   Qi_redundant   QJoggle\n\
 Qsearch_1st    Qtriangulate   QVertex_good\n\
\n\
 T4_trace       Tcheck_often   Tstatistics    Tverify        Tz_stdout\n\
 TFacet_log     TInput_file    TPoint_trace   TMerge_trace   TOutput_file\n\
 TWide_trace    TVertex_stop   TCone_stop\n\
\n\
 Angle_max      Centrum_size   Random_dist    Ucoplanar_max  Wide_outside\n\
";

int halfspace(FILE* rPtr, FILE* wPtr) {
	int curlong, totlong; /* used !qh_NOmem */
	int exitcode, numpoints, dim;
	coordT *points;
	boolT ismalloc;

  	int argc = 3;
	char* argv[3];
	argv[0] = "qhalf";
	argv[1] = "Fp";
	argv[2] = "Fx";

	qh_init_A(rPtr, wPtr, stderr, argc, argv);  /* sets qh qhull_command */
	exitcode= setjmp(qh errexit); /* simple statement for CRAY J916 */
	if (!exitcode) {
		qh NOerrexit = False;
		qh_option("Halfspace", NULL, NULL);
		qh HALFspace= True;    /* 'H'   */
		qh_checkflags(qh qhull_command, hidden_options);
		qh_initflags(qh qhull_command);

		points= qh_readpoints(&numpoints, &dim, &ismalloc);

		//for(int i = 0; i < numpoints; i++)
		//{
		//	for(int j = 0; j < dim; j++)
		//	{
		//		printf("%lf ", points[i* dim + j]);
		//	}
		//	printf("\n");
		//}

		if (dim >= 5) {
			qh_option("Qxact_merge", NULL, NULL);
			qh MERGEexact= True; /* 'Qx' always */
		}
		qh_init_B(points, numpoints, dim, ismalloc);
		qh_qhull();
		qh_check_output();
		qh_produce_output();
		//print_summary();

		if (qh VERIFYoutput && !qh FORCEoutput && !qh STOPpoint && !qh STOPcone)
			qh_check_points();
		exitcode= qh_ERRnone;
	}
	qh NOerrexit= True;  /* no more setjmp */
	#ifdef qh_NOmem
	qh_freeqhull(qh_ALL);
	#else
	qh_freeqhull(!qh_ALL);
	qh_memfreeshort(&curlong, &totlong);
	if (curlong || totlong)
	fprintf(stderr, "qhull internal warning (main): did not free %d bytes of long memory(%d pieces)\n",
		totlong, curlong);
	#endif

	return exitcode;
} /* main */

vector<point_t*> get_extreme_pts(vector<point_t*>& ext_vec)
{
	int dim = ext_vec[0]->dim;
	char file1[MAX_FILENAME_LENG];
	sprintf(file1, "hyperplane_data");
	char file2[MAX_FILENAME_LENG];
	sprintf(file2, "ext_pt");

	// construct the hyperplanes and a feasible point
	vector<hyperplane_t*> utility_hyperplane;
	point_t* normal;
	normal = alloc_point(dim);
	for(int i = 0; i < dim; i++)
		normal->coord[i] = 1;
	utility_hyperplane.push_back( alloc_hyperplane(normal, -1));
	for(int i = 0; i < ext_vec.size(); i++)
	{
		normal = copy(ext_vec[i]);
		utility_hyperplane.push_back( alloc_hyperplane(normal, 0));
	}
	point_t* feasible_pt = find_feasible(utility_hyperplane);

	write_hyperplanes(utility_hyperplane, feasible_pt, file1);
	for(int i = 0; i < utility_hyperplane.size(); i++)
		release_hyperplane(utility_hyperplane[i]);

	// write hyperplanes and the feasible point to file, conduct half space intersection
	FILE* rPtr;
	FILE* wPtr;
	if ((rPtr = fopen(file1, "r")) == NULL)
	{
		fprintf(stderr, "Cannot open the data file.\n");
		exit(0);
	}
	wPtr = (FILE *)fopen(file2, "w");
	halfspace(rPtr, wPtr);
	fclose(rPtr);
	fclose(wPtr);

	//read extreme points
	if ((rPtr = fopen(file2, "r")) == NULL)
	{
		fprintf(stderr, "Cannot open the data file %s.\n", file2);
		exit(0);
	}

	int size;
	vector<point_t*> ext_pts;
	fscanf(rPtr, "%i%i", &dim, &size);
	for (int i = 0; i < size; i++)
	{
		bool allZero = true;
		point_t* p = alloc_point(dim);
		for (int j = 0; j < dim; j++)
		{
			fscanf(rPtr, "%lf", &p->coord[j]);
			if(!isZero(p->coord[j]))
				allZero = false;
		}
		if(allZero)
			release_point(p);
		else
		{
			ext_pts.push_back(p);
			//print_point(p);
		}
	}



	vector<point_t*> new_ext_vec;
	fscanf(rPtr, "%i", &size);
	for (int i = 0; i < size; i++)
	{
		int idx;
		fscanf(rPtr, "%i", &idx);

		if(idx > 0)
			new_ext_vec.push_back(copy(ext_vec[idx - 1]));
	}
	for(int i = 0; i < ext_vec.size(); i++)
	{
		release_point(ext_vec[i]);
	}
	ext_vec = new_ext_vec;

	fclose(rPtr);
	return ext_pts;
}

void print_summary(void) {
	facetT *facet;
	int k;

	printf("\n%d vertices and %d facets with normals:\n",
		qh num_vertices, qh num_facets);

	FORALLfacets{
		for (k = 0; k < qh hull_dim; k++)
		printf("%lf\t", facet->normal[k]);

	printf("%lf\t%d\n", facet->offset, facet->id);
	}

}

void get_hyperplanes(vector<point_t*>& ext_vec, hyperplane_t*& hp, vector<point_t*>& hyperplanes)
{
	vector<int> frame;
	frameConeLP(ext_vec, frame);
	vector<point_t*> new_ext_vec;
	for(int i = 0; i < frame.size(); i++)
		new_ext_vec.push_back(copy(ext_vec[frame[i]]));
	for(int i = 0; i < ext_vec.size(); i ++)
		release_point(ext_vec[i]);
	ext_vec = new_ext_vec;

	int dim = ext_vec[0]->dim;
	
	double offset = 0;
	point_t* normal = alloc_point(dim);
	for(int i = 0; i < dim; i++)
		normal->coord[i] = 0;

	//constuct non-trivial hyperplane
	if(1)
	{
		for(int i = 0; i < ext_vec.size(); i++)
		{
			point_t* minus = scale(-1, ext_vec[i]);
			double len;
			point_t* pi = alloc_point(dim);

			solveLP(ext_vec, minus, len, pi);

			point_t* new_normal = add(normal, pi);

			offset = dot_prod(normal, ext_vec[0]);
			for (int i = 1; i < ext_vec.size(); i++)
			{
				double temp = dot_prod(normal, ext_vec[i]);
				if (temp > offset)
					offset = temp;
			}

			release_point(pi);
			release_point(minus);
			release_point(normal);

			normal = new_normal;

			if(offset < 0 && !isZero(offset))
				break;
		}

		double length = calc_len(normal);
		for(int i = 0; i < dim; i++)
			normal->coord[i] /= -length;
		offset = dot_prod(normal, ext_vec[0]);
		for (int i = 1; i < ext_vec.size(); i++)
		{
			double temp = dot_prod(normal, ext_vec[i]);
			if (temp < offset)
				offset = temp;
		}	
	}

	hp = alloc_hyperplane(normal, offset);

	//for(int i = 0; i < ext_vec.size(); i ++)
	//{
	//	print_point(ext_vec[i]);
	//	double v = dot_prod(normal, ext_vec[i]);
	//	printf("dot_prod: %lf \n", v);
	//}	

	//printf("offset: %lf\n", offset);


	//for(int i = 0; i < ext_vec.size(); i ++)
	//{
	//	print_point(ext_vec[i]);
	//	double v = dot_prod(hp->normal, ext_vec[i]);
	//	//v = compute_intersection_len(hp, ext_vec[i]);
	//	printf("%lf \n", v);
	//	if(v < 0 && !isZero(v))
	//	{
	//		printf("negative! %lf", v);
	//		system("pause");
	//	}
	//}
	//printf("\n");

	int n = ext_vec.size() + 1;


	int curlong, totlong; /* used !qh_NOmem */
	int exitcode;
	boolT ismalloc = True;

	coordT *points;
	//temp_points = new coordT[(orthNum * S->numberOfPoints + 1)*(dim)];
	points = qh temp_malloc = (coordT*)qh_malloc(n*(dim)*sizeof(coordT));

	for (int i = 0; i < ext_vec.size(); i++)
	{
		//double len = compute_intersection_len(hp, ext_vec[i]);
		//printf("%lf %lf\n", len, calc_len(ext_vec[i]));
		//point_t* tmp = scale( len, ext_vec[i]);
		//for (int j = 0; j < dim; j++)
		//	points[i*dim + j] = tmp->coord[j];
		for (int j = 0; j < dim; j++)
			points[i*dim + j] = ext_vec[i]->coord[j];
	}

	for (int i = 0; i < dim; i++)
	{
		points[ext_vec.size()*dim + i] = 0;
	}

	//printf("# of points: %d\n", count);
	qh_init_A(stdin, stdout, stderr, 0, NULL);  /* sets qh qhull_command */
	exitcode = setjmp(qh errexit); /* simple statement for CRAY J916 */

	double minCR;

	if (!exitcode) {

		//qh POSTmerge = True;
		////qh postmerge_centrum = 0.01;
		//qh premerge_cos = 0.995;

		qh_initflags(qh qhull_command);
		qh_init_B(points, n, dim, ismalloc);
		qh_qhull();
		qh_check_output();


		if (qh VERIFYoutput && !qh FORCEoutput && !qh STOPpoint && !qh STOPcone)
			qh_check_points();
		exitcode = qh_ERRnone;

		//qh_vertexneighbors();
		//print_summary();


		//vertexT *vertex;
		//FORALLvertices
		//{
		//	bool isPt = true;
		//	for (int i = 0; i < dim; i++)
		//	{
		//		if (!isZero(vertex->point[i]))
		//		{
		//			isPt = false;
		//			break;
		//		}
		//	}

		//	if (isPt)
		//	{
		//		facetT *facet, **facetp;
		//		FOREACHfacet_(vertex->neighbors)
		//		{
		//			point_t* normal = alloc_point(dim);
		//			for (int j = 0; j < dim; j++)
		//				normal->coord[j] = facet->normal[j];
		//			hyperplanes.push_back(normal);
		//		}
		//		break;
		//	}
		//}
		facetT *facet;
		FORALLfacets{

			if(isZero(facet->offset))
			{
				point_t* normal = alloc_point(dim);
				for (int j = 0; j < dim; j++)
					normal->coord[j] = facet->normal[j];
				hyperplanes.push_back(normal);
			}

		}

	}

	qh NOerrexit = True;  /* no more setjmp */
#ifdef qh_NOmem
	qh_freeqhull(True);
#else
	qh_freeqhull(False);
	qh_memfreeshort(&curlong, &totlong);
	if (curlong || totlong)
		fprintf(stderr, "qhull internal warning (main): did not free %d bytes of long memory(%d pieces)\n",
			totlong, curlong);
#endif

}

int hyperplane_dom(point_t* p_i, point_t* p_j, vector<point_t*> ext_pts)
{
	int dim = p_i->dim;

	point_t* normal = sub(p_i, p_j);
	
	int below_count = 0;

	for(int i = 0; i < ext_pts.size(); i++)
	{
		point_t* ext_pt = ext_pts[i];
		double v = dot_prod(normal, ext_pt);

		if(v < 0 & !isZero(v))
		{
			below_count++;
			break;
		}
	}
	release_point(normal);

	if (below_count == 0)
		return 1;
	else
		return 0;
}

int conical_hull_dom(point_t* p_i, point_t* p_j, hyperplane_t* hp, vector<point_t*> hyperplanes, vector<point_t*> ext_vec)
{
	int dim = p_i->dim;
	int dominate;

	point_t* minus = sub(p_j, p_i);

	double len = compute_intersection_len(hp, minus);

	if (len < 1 && len > 0 || isZero(len - 1) || isZero(len))
	//if(1)
	{
		bool all_below = true;
		for(int i = 0; i < hyperplanes.size(); i++)
		{
			point_t* normal = hyperplanes[i];
			double v = dot_prod(normal, minus);

			if(v > 0 && !isZero(v))
			{
				all_below = false;
				break;
			}
		}
		if (all_below)
		{
			dominate = 1;
			if(len > 1 )
			{
				printf("bug\n");

			//	printf("hyperplanes:\n");
			//	for(int i = 0; i < hyperplanes.size(); i++)
			//		print_point(hyperplanes[i]);

			//	print_point(minus);
			//	printf("below 1 - %lf\n", dot_prod(minus, hyperplanes[0]));
			//	printf("below 2 - %lf\n", dot_prod(minus, hyperplanes[1]));
			//	

			//	print_point(hp->normal);
			//	printf("offset: %lf\n", hp->offset);


			//	if(insideCone(ext_vec, minus))
			//		printf("in\n");
			//	else
			//		printf("out\n");

			//	printf("len: %lf\n", len);

			//	for(int i =0; i < ext_vec.size(); i++)
			//	{
			//		print_point(ext_vec[i]);
			//		//printf("dot_prod: %lf, len: %lf\n", dot_prod(hp->normal, ext_vec[i]), compute_intersection_len(hp,  ext_vec[i]) );
			//	}
			//	system("pause");
			}
		}
		else
			dominate = 0;
	}
	else
	{
		dominate = 0;
		//printf("pruned!\n");
	}

	release_point(minus);

	return dominate;
}

int dom(point_t* p_i, point_t* p_j, vector<point_t*> ext_pts, hyperplane_t* hp, vector<point_t*> hyperplanes, vector<point_t*> ext_vec, int dom_option)
{
	if(dom_option == HYPER_PLANE)
		return hyperplane_dom(p_i, p_j, ext_pts);
	else
		return conical_hull_dom(p_i, p_j, hp, hyperplanes, ext_vec);
}

double get_rrbound_approx(vector<point_t*> ext_pts)
{
	if(ext_pts.size() == 0)
		return 1;
	
	int dim = ext_pts[0]->dim;

	double* max = new double[dim];
	double* min = new double[dim];

	for(int i = 0; i < dim; i++)
	{
		max[i] = ext_pts[0]->coord[i];
		min[i] = ext_pts[0]->coord[i];
	}

	for(int i = 1; i < ext_pts.size(); i++)
	{
		point_t* pt = ext_pts[i];

		for(int j = 0; j < dim; j++)
		{
			if(pt->coord[j] > max[j])
				max[j] = pt->coord[j];
			else if(pt->coord[j] < min[j])
				min[j] = pt->coord[j];
		}
	}

	double bound = 0;

	for(int i = 0; i < dim; i++)
		bound += max[i] - min[i];

	bound *= ext_pts[0]->dim;

	delete [] max;
	delete [] min;

	return bound < 1? bound: 1;
}

double get_rrbound_exact(vector<point_t*> ext_pts)
{
	if(ext_pts.size() == 0)
		return 1;

	double max = 0;

	for(int i = 0; i < ext_pts.size(); i++)
	{
		for(int j = i+1; j < ext_pts.size(); j++)
		{
			double v = calc_l1_dist(ext_pts[i], ext_pts[j]);
			if(v > max)
				max = v;
		}
	}

	max *= ext_pts[0]->dim;

	return max < 1? max : 1;
}

void sql_pruning(point_set_t* P, vector<int>& C_idx, vector<point_t*>& ext_vec, double& rr, int stop_option, int dom_option)
{
	int dim = P->points[0]->dim;

	vector<point_t*> ext_pts;
	vector<point_t*> hyperplanes;
	hyperplane_t* hp = NULL;
	
	if(dom_option == HYPER_PLANE)
		ext_pts = get_extreme_pts(ext_vec);
	else
	{
		get_hyperplanes(ext_vec, hp, hyperplanes); 
		if(stop_option != NO_BOUND)
			ext_pts = get_extreme_pts(ext_vec);
	}
	if(stop_option == EXACT_BOUND)
		rr = get_rrbound_exact(ext_pts);
	else if (stop_option == APPROX_BOUND)
		rr = get_rrbound_approx(ext_pts);
	else 
		rr = 1;

	//printf("extreme vectors:\n");
	//for(int i = 0; i < ext_vec.size(); i++)
	//	print_point(ext_vec[i]);
	//printf("hyperplanes:\n");
	//for(int i = 0; i < hyperplanes.size(); i++)
	//	print_point(hyperplanes[i]);
	//printf("H: offset - %lf\n", hp->offset);
	//print_point(hp->normal);


	int* sl = new int[C_idx.size()];
	int index = 0;

	for (int i = 0; i < C_idx.size(); ++i)
	{

		int dominated = 0;
		point_t* pt = P->points[C_idx[i]];

		// check if pt is dominated by the skyline so far   
		for (int j = 0; j < index && !dominated; ++j)
		{

			if(dom(P->points[ sl[j] ], pt, ext_pts, hp, hyperplanes, ext_vec, dom_option))
				dominated = 1;
		}

		if (!dominated)
		{
			// eliminate any points in current skyline that it dominates
			int m = index;
			index = 0;
			for (int j = 0; j < m; ++j)
			{

				if(!dom(pt, P->points[sl[j]], ext_pts, hp, hyperplanes, ext_vec, dom_option))
					sl[index++] = sl[j];
			}

			// add this point as well
			sl[index++] = C_idx[i];
		}
	}

	C_idx.clear();
	for(int i = 0; i < index; i++)
		C_idx.push_back(sl[i]);

	delete[] sl;

	if(dom_option == HYPER_PLANE)
	{
		for(int i = 0; i < ext_pts.size(); i++)
			release_point(ext_pts[i]);
	}
	else
	{
		release_hyperplane(hp);
		for(int i = 0; i < hyperplanes.size(); i++)
			release_point(hyperplanes[i]);
	}
	
}

void rtree_pruning(point_set_t* P, vector<int>& C_idx, vector<point_t*>& ext_vec, double& rr,  int stop_option, int dom_option)
{
	vector<point_t*> ext_pts;
	vector<point_t*> hyperplanes;
	hyperplane_t* hp = NULL;
	
	if(dom_option == HYPER_PLANE)
		ext_pts = get_extreme_pts(ext_vec);
	else
	{
		get_hyperplanes(ext_vec, hp, hyperplanes); 
		if(stop_option != NO_BOUND)
			ext_pts = get_extreme_pts(ext_vec);
	}
	if(stop_option == EXACT_BOUND)
		rr = get_rrbound_exact(ext_pts);
	else if (stop_option == APPROX_BOUND)
		rr = get_rrbound_approx(ext_pts);
	else 
		rr = 1;

	rtree_info *aInfo;
	aInfo = (rtree_info *)malloc(sizeof(rtree_info));
	memset(aInfo, 0, sizeof(rtree_info));

	aInfo->m = 18;
	aInfo->M = 36;
	aInfo->dim = P->points[0]->dim;
	aInfo->reinsert_p = 27;
	aInfo->no_histogram = C_idx.size();

	node_type *root = contructRtree(P, C_idx, aInfo);

	priority_queue<node_type*, vector<node_type*>, nodeCmp> heap;

	heap.push(root);

	int* sl = new int[C_idx.size()];
	int index = 0;
	int dim = aInfo->dim;

	while (!heap.empty())
	{
		node_type* n = heap.top();
		heap.pop();

	    
		if (n->attribute != LEAF)
		{
			
			int dominated = 0;
			
			point_t* TRpt = alloc_point(dim);
			for (int i = 0; i < dim; i++)
				TRpt->coord[i] = n->b[i];

			// check if TRpt is dominated by the skyline so far   
			for (int j = 0; j < index && !dominated; ++j)
			{
				
				if(dom(P->points[ sl[j] ], TRpt, ext_pts, hp, hyperplanes, ext_vec, dom_option))
					dominated = 1;

			}


			if (!dominated)
			{

				for (int i = 0; i < aInfo->M - n->vacancy; i++)
				{
					//int child_dominated = 0;
					//for (int i = 0; i < dim; i++)
					//	TRpt->coord[i] = n->ptr[i]->b[i];
	
					//for (int j = 0; j < index && !dominated; ++j)
					//	if (hyperplane_dom(P->points[ sl[j] ], TRpt, ext_pts))
					//		child_dominated = 1;
					//
					//if(!child_dominated)
					heap.push(n->ptr[i]);
				}
			}
				
		}
		else
		{
			int idx = n->id;
			//S = updateS(id, C, S, V);

			int dominated = 0;
			for (int j = 0; j < index && !dominated; ++j)
			{
				if(dom(P->points[ sl[j] ], P->points[ C_idx[idx] ], ext_pts, hp, hyperplanes, ext_vec, dom_option))
					dominated = 1;
			}
			if (dominated)
				continue;

			// eliminate any points in current skyline that it dominates
			int m = index;
			index = 0;
			for (int j = 0; j < m; ++j)
			{
				if(!dom(P->points[C_idx[idx]], P->points[sl[j]], ext_pts, hp, hyperplanes, ext_vec, dom_option))
					sl[index++] = sl[j];
			}

			// add this point as well
			sl[index++] = C_idx[idx];
		}
	}
	
	C_idx.clear();
	for(int i = 0; i < index; i++)
		C_idx.push_back(sl[i]);

	delete[] sl;

	free(aInfo);

	if(dom_option == HYPER_PLANE)
	{
		for(int i = 0; i < ext_pts.size(); i++)
			release_point(ext_pts[i]);
	}
	else
	{
		release_hyperplane(hp);
		for(int i = 0; i < hyperplanes.size(); i++)
			release_point(hyperplanes[i]);
	}
}
