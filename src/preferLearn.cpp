#include <algorithm>
#include "preferLearn.h"

int get_current_best_pt(point_set_t* P, vector<int>& C_idx, vector<point_t*>& ext_vec)
{
	int mycounter = 0;
	int dim = P->points[0]->dim;

	vector<point_t*> ext_pts;

	//for(int i = 0; i < ext_vec.size(); i++)
	//	print_point(ext_vec[i]);

	ext_pts = get_extreme_pts(ext_vec);

	//for(int i = 0; i < ext_vec.size(); i++)
	//	print_point(ext_vec[i]);
	point_t* mean = alloc_point(dim);

	for(int i = 0; i < dim; i++)
	{
		mean->coord[i] = 0;
	}
	for(int i = 0; i < ext_pts.size(); i++)
	{
		for(int j = 0; j < dim; j++)
			mean->coord[j] += ext_pts[i]->coord[j];
	}
	for(int i = 0; i < dim; i++)
	{
		mean->coord[i] /= ext_pts.size();
	}
	//print_point(mean);

	int best_pt_idx;
	double max = 0;
	for(int i = 0; i < C_idx.size(); i++)
	{
		point_t* pt = P->points[C_idx[i]];

		double v = dot_prod(pt, mean);
		if(v > max)
		{
			max = v;
			best_pt_idx =  C_idx[i];
		}
	}

	for(int i = 0; i < ext_pts.size(); i++)
		release_point(ext_pts[i]);

	return best_pt_idx;
}

vector<int> generate_S(point_set_t* P, vector<int>& C_idx, int s, int current_best_idx, int& last_best, vector<int>& frame, int cmp_option)
{
	vector<int> S;

	if(cmp_option == RANDOM)
	{
		while(S.size() < s && S.size() < C_idx.size())
		{
			int idx = rand() % C_idx.size();

			bool isNew = true;
			for(int i = 0; i < S.size(); i++)
			{
				if(S[i] == idx)
				{
					isNew = false;
					break;
				}
			}
			if(isNew)
				S.push_back(idx);
		}
	}
	else if(cmp_option == SIMPLEX)
	{
		if(last_best != current_best_idx || frame.size() == 0)
		{
			vector<point_t*> rays;

			int best_i = -1;
			for(int i = 0; i < P->numberOfPoints; i++)
			{
				if(i == current_best_idx)
				{
					best_i = i;
					continue;
				}

				point_t* best = P->points[current_best_idx];

				point_t* newRay = sub(P->points[i], best);

				rays.push_back(newRay);
			}

			frameConeFastLP(rays, frame);
		
			for(int i = 0; i < frame.size(); i++)
			{
				if(frame[i] >= current_best_idx)
					frame[i]++;

				//S[i] = C_idx[S[i]];
			}

			for(int i = 0; i < rays.size(); i++)
				release_point(rays[i]);
		}

		//printf("current_best: %d, frame:", P->points[current_best_idx]->id);
		//for(int i = 0; i < frame.size(); i++)
		//	printf("%d ", P->points[frame[i]]->id);
		//printf("\n");

		//S.push_back(best_i);
		for(int j = 0; j < C_idx.size(); j++)
		{
			if(C_idx[j] == current_best_idx)
			{
				S.push_back(j);
				break;
			}
		}

		for(int i = 0; i < frame.size() && S.size() < s; i++)
		{
			for(int j = 0; j < C_idx.size() && S.size() < s; j++)
			{
				if(C_idx[j] == current_best_idx)
					continue;

				if(C_idx[j] == frame[i])
					S.push_back(j);
			}
		}

		if (S.size() < s && C_idx.size() > s)
		{
			for (int j = 0; j < C_idx.size(); j++)
			{
				bool noIn = true;
				for (int i = 0; i < S.size(); i++)
				{
					if (j == S[i])
						noIn = false;
				}
				if (noIn)
					S.push_back(j);

				if (S.size() == s)
					break;
			}
		}
	}
	else
	{
		vector<point_t*> rays;

		int best_i = -1;
		for(int i = 0; i < C_idx.size(); i++)
		{
			if(C_idx[i] == current_best_idx)
			{
				best_i = i;
				continue;
			}

			point_t* best = P->points[current_best_idx];

			point_t* newRay = sub(P->points[C_idx[i]], best);

			rays.push_back(newRay);
		}

		partialConeFastLP(rays, S, s - 1);
		if(S.size() > s - 1)
			S.resize(s - 1);
		for(int i = 0; i < S.size(); i++)
		{
			if(S[i] >= best_i)
				S[i]++;

			//S[i] = C_idx[S[i]];
		}
		S.push_back(best_i);


		for(int i = 0; i < rays.size(); i++)
			release_point(rays[i]);
	}
	return S;
}


int show_to_user(point_set_t* P, int p_idx, int q_idx)
{

	int option = 0;

	while (option != 1 && option != 2)
	{
		printf("Please choose the car you favor more:\n");
		printf("--------------------------------------------------------\n");
		printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
		printf("--------------------------------------------------------\n");
		printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Option 1", P->points[p_idx]->coord[0], P->points[p_idx]->coord[1], P->points[p_idx]->coord[2], P->points[p_idx]->coord[3]);
		printf("--------------------------------------------------------\n");
		printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Option 2", P->points[q_idx]->coord[0], P->points[q_idx]->coord[1], P->points[q_idx]->coord[2], P->points[q_idx]->coord[3]);
		printf("--------------------------------------------------------\n");
		printf("Your choice:");
		scanf("%d", &option);
	}

	return option;
}

void update_ext_vec(FILE *wPtr, point_set_t* P_car, point_set_t* skyline_proc_P, vector<int>& C_idx, int s, vector<point_t*>& ext_vec, int& current_best_idx, int& last_best, vector<int>& frame, int cmp_option)
{
	
	vector<int> S = generate_S(skyline_proc_P, C_idx, s, current_best_idx, last_best, frame, cmp_option);
	
	if (S.size() != 2)
	{
		printf("invalid option numbers\n");
		exit(0);
	}
	
	//printf("comp: %d %d\n", skyline_proc_P->points[C_idx[S[0]]]->id, skyline_proc_P->points[C_idx[S[1]]]->id);
	
	int max_i = show_to_user(P_car, skyline_proc_P->points[C_idx[S[0]]]->id, skyline_proc_P->points[C_idx[S[1]]]->id) - 1;

	fprintf(wPtr, "%d\t%d\t%d\n", skyline_proc_P->points[C_idx[S[0]]]->id, skyline_proc_P->points[C_idx[S[1]]]->id, skyline_proc_P->points[C_idx[S[max_i]]]->id);

	last_best = current_best_idx;
	current_best_idx = C_idx[S[max_i]];
	//if(current_best_idx == S[max_i])

	//print_point(skyline_proc_P->points[C_idx[S[0]]]);
	//print_point(skyline_proc_P->points[C_idx[S[1]]]);

	for (int i = 0; i < S.size(); i++)
	{
		if (max_i == i)
			continue;

		//print_point(skyline_proc_P->points[C_idx[S[i]]]);
		//print_point(skyline_proc_P->points[C_idx[S[max_i]]]);

		point_t* tmp = sub(skyline_proc_P->points[C_idx[S[i]]], skyline_proc_P->points[C_idx[S[max_i]]]);
		C_idx[S[i]] = -1;

		point_t* new_ext_vec = scale(1 / calc_len(tmp), tmp);

		release_point(tmp);
		ext_vec.push_back(new_ext_vec);
	}


	vector<int> newC_idx;
	for (int i = 0; i < C_idx.size(); i++)
	{
		if (C_idx[i] >= 0)
			newC_idx.push_back(C_idx[i]);
	}
	C_idx = newC_idx;
}

point_t* max_utility(FILE *wPtr, point_set_t* P_car, int cmp_option)
{
	int s = 2;
	int prune_option = RTREE;

	point_set_t* proc_P = process_car(P_car);
	point_set_t* skyline_proc_P = skyline_point(proc_P);

	int dim = skyline_proc_P->points[0]->dim;

	vector<int> C_idx;
	for (int i = 0; i < skyline_proc_P->numberOfPoints; i++)
		C_idx.push_back(i);

	vector<point_t*> ext_vec;
	for (int i = 0; i < dim; i++)
	{
		point_t* e = alloc_point(dim);
		for (int j = 0; j < dim; j++)
		{
			if (i == j)
				e->coord[j] = -1;
			else
				e->coord[j] = 0;
		}
		ext_vec.push_back(e);
	}

	int current_best_idx = -1;
	int last_best = -1;
	vector<int> frame;

	current_best_idx = get_current_best_pt(skyline_proc_P, C_idx, ext_vec);

	// if not skyline
	//sql_pruning(P, C_idx, ext_vec);

	double rr = 1;

	
	int Qcount = 0;
	printf("%Qcount: %d, Csize: %d\n", Qcount, C_idx.size());
	fprintf(wPtr, "%d\t%d\t", Qcount, C_idx.size());

	//printf("bound:\n");
	while (C_idx.size()> 1)
	{
		Qcount++;
		sort(C_idx.begin(), C_idx.end()); // prevent select two different points after different skyline algorithms
		update_ext_vec(wPtr, P_car, skyline_proc_P, C_idx, s, ext_vec, current_best_idx, last_best, frame, cmp_option);

		if (C_idx.size() == 1) // || global_best_idx == current_best_idx
			break;


		rtree_pruning(skyline_proc_P, C_idx, ext_vec, rr, NO_BOUND, HYPER_PLANE);
		printf("%Qcount: %d, Csize: %d\n", Qcount, C_idx.size());
		fprintf(wPtr, "%d\t%d\t", Qcount, C_idx.size());
	}
	fprintf(wPtr, "%d\t%d\n", Qcount, C_idx.size());
	
	point_t* result = P_car->points[skyline_proc_P->points[get_current_best_pt(skyline_proc_P, C_idx, ext_vec)]->id];

	for (int i = 0; i < ext_vec.size(); i++)
		release_point(ext_vec[i]);

	return result;
}

bool inRange(int lb, int ub, int target)
{
	if (target > ub|| target < lb)
		return false;
	else
		return true;
}

point_set_t* selectRange(FILE *wPtr, point_set_t* P)
{
	bool finish = false;

	vector<point_t*> candidate;
	while (!finish)
	{
		candidate.clear();
		int lb[4], ub[4];

		printf("Indicate your affordable price range (1000USD ~ 50000USD): ");
		scanf("%d %d", &lb[0], &ub[0]);
		printf("Indicate your affordable year range (2001 ~ 2017): ");
		scanf("%d %d", &lb[1], &ub[1]);
		printf("Indicate your affordable power range (50 ~ 400): ");
		scanf("%d %d", &lb[2], &ub[2]);
		printf("Indicate your affordable used KM range (10000 ~ 150000): ");
		scanf("%d %d", &lb[3], &ub[3]);

		for (int i = 0; i < 4; i++)
		{
			fprintf(wPtr, "%d\t%d\n", lb[i], ub[i]);
		}

		for (int i = 0; i < P->numberOfPoints; i++)
		{
			bool isValid = true;
			for (int j = 0; j < 4; j++)
			{
				if (!inRange(lb[j], ub[j], P->points[i]->coord[j]))
					isValid = false;
			}
			if (isValid)
				candidate.push_back(P->points[i]);

			if (candidate.size() == 1000)
				break;
		}

		if (candidate.size() == 1000)
			finish = true;
		else
			printf("Too few tuples. Try larger ranges again!\n");
	}

	point_set_t* new_P = alloc_point_set(1000);
	
	for (int i = 0; i < 1000; i++)
	{
		new_P->points[i] = copy(candidate[i]);
		new_P->points[i]->id = i;
	}

	release_point_set(P, true);

	return new_P;
}


void userStudy()
{
	printf("------------Welcome to my User Study-----------------\n");
	printf("This is a used car database with 1000 used car.\n");
	printf("Four attributes are selected to present each car: \n\tprice, year purchased, power, used KM.\n");

	printf("You will be presented two cars at a time.\n");
	printf("Please selected the car you favor more each time.\n");
	printf("Try to be consistant and correct as much as possible.\n");

	point_set_t* P = read_points("car.txt");

	char outputfile[MAX_FILENAME_LENG];
	char name[MAX_FILENAME_LENG];
	printf("\nPlease input your name: ");
	scanf("%s", name);
	
	FILE *wPtr;
	sprintf(outputfile, "output/%s.txt", name);
	wPtr = (FILE *)fopen(outputfile, "w");


	P = selectRange(wPtr, P);

	//fprintf(wPtr, "\nRandom\n");
	//point_t* p1 = max_utility(wPtr, P, RANDOM);

		
	//fprintf(wPtr, "\nSIMPLEX\n");
	point_t* p2 = max_utility(wPtr, P, SIMPLEX);
	
	printf("Your Favorite Car:\n");
	print_point(p2);
	fprintf(wPtr, "%d\n", p2->id);

	fprintf(wPtr, "\n");

	fclose(wPtr);
}
