#include <algorithm>
#include "preferLearn.h"

int get_current_best_pt(point_set_t* P, vector<int>& C_idx, vector<point_t*>& ext_vec);
vector<int> generate_S(point_set_t* P, vector<int>& C_idx, int s, int current_best_idx, int& last_best, vector<int>& frame, int cmp_option);

class Point {
public:
    Point(point_t *point): point(point) {
    }

    int dim() {
        return point->dim;
    }

    double get(int idx){
        return point->coord[idx];
    }

    point_t * raw_ptr() {
        return point;
    }

private:
    point_t *point;
};

class Dataset {
public:
    Dataset(string filename) {
        allPoints = read_points((char *)filename.c_str());
    }

    ~Dataset(){
        release_point_set(allPoints, true);
    }

    // ranges.size() == dimension of points
    // mask.size() == ranges.size(), mask[i] = 1 if the ith attribute is used, 0 if not used.
    vector<Point> selectCandidates(vector<pair<int, int>> ranges, vector<int> mask, int maxPoints){
        vector<Point> candidates;
        int new_dim = 0;
        for(int i = 0; i < mask.size(); ++i){
            new_dim += mask[i];
        }
        for(int i = 0; i < allPoints->numberOfPoints; ++i){
            point_t *point = allPoints->points[i];
            bool isValid = true;
            for(int j = 0; j < ranges.size(); ++j){
                if(mask[j] && !inRange(ranges[j], point->coord[j])){
                    isValid = false;
                    break;
                }
            }
            if(isValid){
                point_t *new_point = alloc_point(new_dim);
                for(int i = 0, j = 0; i < mask.size(); ++i){
                    if(mask[i]){
                        new_point->coord[j++] = point->coord[i];
                    }
                }
                candidates.push_back(Point(new_point));
                if(candidates.size() >= maxPoints)
                    break;
            }
        }
        return candidates;
    }

    bool inRange(pair<int, int> &range, int val){
        return range.first <= val && val <= range.second;
    }

private:
    point_set_t *allPoints;
};

point_set_t *normalize_points(point_set_t *P, vector<int> smallerBetter){

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
            if(smallerBetter[j])
                norm_P->points[i]->coord[j] = 1 - norm_P->points[i]->coord[j];
    }


    return norm_P;

}

class AlgorithmRunner {
public:
    AlgorithmRunner(vector<Point> &candidates, vector<int> &smallerBetter, int cmp_option) {
        this->candidates = alloc_point_set(candidates.size());
        for(int i = 0; i < candidates.size(); ++i){
            point_t *point = candidates[i].raw_ptr();
            this->candidates->points[i] = point;
            this->candidates->points[i]->id = i;
        }

        this->cmp_option = cmp_option;
        s = 2;
        prune_option = RTREE;
        current_best_idx = -1;
        last_best = -1;
        rr = 1;
        points_norm = normalize_points(this->candidates, smallerBetter); // new points
        skyline = skyline_point(points_norm); // does not copy points

        int dim = skyline->points[0]->dim;
        for(int i = 0; i < skyline->numberOfPoints; ++i){
            C_idx.push_back(i);
        }
        for(int i = 0; i < dim; ++i){
            point_t *e = alloc_point(dim);
            for(int j = 0; j < dim; ++j){
                e->coord[j] = i == j ? -1 : 0;
            }
            ext_vec.push_back(e);
        }
        
        if(cmp_option != RANDOM){
            current_best_idx = get_current_best_pt(skyline, C_idx, ext_vec);
        }
    }

    ~AlgorithmRunner() {
        for(int i = 0; i < ext_vec.size(); ++i)
            release_point(ext_vec[i]);
        release_point_set(candidates, false);
        release_point_set(skyline, false);
        release_point_set(points_norm, true);
    }

    bool isFinished() {
        return C_idx.size() <= 1;
    }

    vector<Point> nextPair(){
        sort(C_idx.begin(), C_idx.end());
        S = generate_S(skyline, C_idx, s, current_best_idx, last_best, frame, cmp_option);
        if(S.size() != 2){
            printf("invalid option numbers");
            exit(0);
        }

        vector<Point> points;
        for(int i = 0; i < S.size(); ++i){
            int id = skyline->points[C_idx[S[i]]]->id;
            Point p(candidates->points[id]);
            points.push_back(p);
        }

        return points;
    }

    Point getResult() {
        int idx = get_current_best_pt(skyline, C_idx, ext_vec);
        int id = skyline->points[idx]->id;
        Point result(candidates->points[id]);
        return result;
    }

    void choose(int option){
        int max_i = option - 1;
        last_best = current_best_idx;
        current_best_idx = C_idx[S[max_i]];

        for(int i = 0; i < S.size(); ++i){
            if(max_i == i) continue;
            point_t *tmp = sub(skyline->points[C_idx[S[i]]], skyline->points[C_idx[S[max_i]]]);
            C_idx[S[i]] = -1;
            point_t *new_ext_vec = scale(1 / calc_len(tmp), tmp);
            release_point(tmp);
            ext_vec.push_back(new_ext_vec);
        }

        int j = 0;
        for(int i = 0; i < C_idx.size(); ++i){
            if(C_idx[i] >= 0)
                C_idx[j++] = C_idx[i];
        }
        C_idx.resize(j);
        if(C_idx.size() > 1){
            rtree_pruning(skyline, C_idx, ext_vec, rr, NO_BOUND, HYPER_PLANE);
        }
    }

    int numLeftPoints() {
        return C_idx.size();
    }

    vector<int> getCandidatesIndices() {
        sort(C_idx.begin(), C_idx.end());
        vector<int> indices;
        for(int i = 0; i < C_idx.size(); ++i){
            int id = skyline->points[C_idx[i]]->id;
            indices.push_back(id);
        }
        return indices;
    }

private:
    point_set_t *candidates;
    int cmp_option;
    int s;
    int prune_option;
    point_set_t *points_norm;
    point_set_t *skyline;
    vector<int> C_idx;
    vector<point_t *> ext_vec;
    int current_best_idx;
    int last_best;
    vector<int> frame;
    double rr;
    vector<int> S;
};

vector<pair<int, int>> getRanges(){
    vector<pair<int, int>> ranges(4);
    cout << "Indicate your affordable price range (1000USD ~ 50000USD): ";
    cin >> ranges[0].first >> ranges[0].second;
    cout << "Indicate your affordable year range (2001 ~ 2017): ";
    cin >> ranges[1].first >> ranges[1].second;
    cout << "Indicate your affordable power range (50 ~ 400): ";
    cin >> ranges[2].first >> ranges[2].second;
    cout << "Indicate your affordable used KM range (10000 ~ 150000): ";
    cin >> ranges[3].first >> ranges[3].second;
    return ranges;
}

int getOption(Point &p1, Point &p2, vector<string> usedAttributes) {
    int option = 0;
    while(option != 1 && option != 2){
        printf("Please choose the car you favor more:\n");
        printf("--------------------------------------------------------\n");
        for(int i = 0; i < usedAttributes.size(); ++i){
            printf("%10s:\t%10.0f\tvs\t%10.0f\n", usedAttributes[i].c_str(), p1.get(i), p2.get(i));
        }
        printf("Your choice:");
        scanf("%d", &option);
    }
    return option;
}

void releasePoints(vector<Point> &points){
    for(auto &p : points){
        point_t *ptr = p.raw_ptr();
        release_point(ptr);
    }
}

void algorithmRunner() {
    Dataset dataset("car.txt");
    vector<Point> candidates;
    vector<string> attributes { "Price", "Year", "PowerPS", "Used KM" };
    vector<int> smallerBetter = {1, 0, 0, 1};
    vector<int> mask(4);
    cout << "Input attribute mask: ";
    for(int i = 0; i < 4; ++i)
        cin >> mask[i];
    vector<string> usedAttributes;
    vector<int> currSmallerBetter;
    int dim = 0;
    for(int i = 0; i < mask.size(); ++i){
        if(mask[i]){
            usedAttributes.push_back(attributes[i]);
            currSmallerBetter.push_back(smallerBetter[i]);
            ++dim;
        }
    }
    int maxPoints;
    cout << "Enter maximum number of points: ";
    cin >> maxPoints;
    do {
        vector<pair<int, int>> ranges {
            {1000, 50000}, {2001, 2017}, {50, 400}, {10000, 150000}
        };
        candidates = dataset.selectCandidates(ranges, mask, maxPoints);
        if(candidates.size() < 1000){
            cout << "Too few tuples. Try larger ranges again!" << endl;
            releasePoints(candidates);
        }
        else
            break;
    }while(true);
    AlgorithmRunner runner(candidates, currSmallerBetter, SIMPLEX);
    int Qcount = 0;
    vector<int> prevIndices = runner.getCandidatesIndices();
    while(!runner.isFinished()){
        cout << "Candidates left: " << endl;
        for(int i: prevIndices){
            point_t *point = candidates[i].raw_ptr();
            for(int j = 0; j < point->dim; ++j){
                printf("%10.0f\t", point->coord[j]);
            }
            printf("\n");
        }
        vector<Point> points = runner.nextPair();
        cout << "Question No. " << Qcount << ". No. of Points Left: " << runner.numLeftPoints() << endl;
        int option = getOption(points[0], points[1], usedAttributes);
        runner.choose(option);
        printf("Points pruned: \n");
        vector<int> currIndices = runner.getCandidatesIndices();
        vector<int> prunedIndices;
        for(int i = 0, j = 0; i < prevIndices.size() || j < currIndices.size(); ){
            if(j >= currIndices.size() || prevIndices[i] < currIndices[j]){
                prunedIndices.push_back(prevIndices[i]);
                ++i;
            }else{ // prevIndices[i] == currIndices[j]
                ++i;
                ++j;
            }
        }
        for(int i: prunedIndices){
            point_t *point = candidates[i].raw_ptr();
            for(int j = 0; j < point->dim; ++j){
                printf("%10.0f\t", point->coord[j]);
            }
            printf("\n");
        }
        prevIndices = currIndices;
    }
    Point result = runner.getResult();
    cout << "Your favorite car is: " << endl;
    print_point(result.raw_ptr());
    releasePoints(candidates);
}

#ifdef EMSCRIPTEN

#include <emscripten/bind.h>
using namespace emscripten;

typedef pair<int, int> Range;

EMSCRIPTEN_BINDINGS(my_module) {
    class_<Point>("Point")
        .function("dim", &Point::dim)
        .function("get", &Point::get)
    ;
    register_vector<Point>("Points");

    value_array<Range>("Range")
        .element(&Range::first)
        .element(&Range::second)
        ;
    
    register_vector<Range>("Ranges");
    register_vector<int>("VectorInt");

    class_<Dataset>("Dataset")
        .constructor<string>()
        .function("selectCandidates", &Dataset::selectCandidates)
        ;

    class_<AlgorithmRunner>("AlgorithmRunner")
        .constructor<vector<Point> &, vector<int> &, int>()
        .function("isFinished", &AlgorithmRunner::isFinished)
        .function("nextPair", &AlgorithmRunner::nextPair)
        .function("getResult", &AlgorithmRunner::getResult)
        .function("choose", &AlgorithmRunner::choose)
        .function("numLeftPoints", &AlgorithmRunner::numLeftPoints)
        .function("getCandidatesIndices", &AlgorithmRunner::getCandidatesIndices)
        ;

    emscripten::function("releasePoints", &releasePoints);
}

#else

int main() {
    algorithmRunner();
    return 0;
}

#endif
