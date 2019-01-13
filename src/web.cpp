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
        cout << "Reading dataset " << filename << endl;
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

class AlgorithmRunner {
public:
    AlgorithmRunner(vector<Point> &candidates) {
        this->candidates = alloc_point_set(candidates.size());
        for(int i = 0; i < candidates.size(); ++i){
            point_t *point = candidates[i].raw_ptr();
            this->candidates->points[i] = copy(point);
            this->candidates->points[i]->id = i;
        }
        cmp_option = SIMPLEX;
        s = 2;
        prune_option = RTREE;
        current_best_idx = -1;
        last_best = -1;
        rr = 1;
        Qcount = 0;
        proc_P = process_car(this->candidates);
        skyline = skyline_point(proc_P);

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
        release_point_set(candidates, true);
        release_point_set(skyline, false);
        release_point_set(proc_P, true);
    }

    bool isFinished() {
        return C_idx.size() <= 1;
    }

    vector<Point> nextPair(){
        ++Qcount;
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
            printf("%Qcount: %d, Csize: %d\n", Qcount, C_idx.size());
        }
    }

    int numLeftPoints() {
        return C_idx.size();
    }

private:
    point_set_t *candidates;
    int cmp_option;
    int s;
    int prune_option;
    point_set_t *proc_P;
    point_set_t *skyline;
    vector<int> C_idx;
    vector<point_t *> ext_vec;
    int current_best_idx;
    int last_best;
    vector<int> frame;
    double rr;
    int Qcount;
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

int getOption(Point &p1, Point &p2) {
    int option = 0;
    while(option != 1 && option != 2){
        printf("Please choose the car you favor more:\n");
        printf("--------------------------------------------------------\n");
        printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
        printf("--------------------------------------------------------\n");
        printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Option 1", p1.get(0), p1.get(1), p1.get(2), p1.get(3));
        printf("--------------------------------------------------------\n");
        printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Option 2", p2.get(0), p2.get(1), p2.get(2), p2.get(3));
        printf("--------------------------------------------------------\n");
        printf("Your choice:");
        scanf("%d", &option);
    }
    return option;
}

void algorithmRunner() {
    Dataset dataset("car.txt");
    vector<Point> candidates;
    do {
        auto ranges = getRanges();
        vector<int> mask {1, 1, 1, 1};
        candidates = dataset.selectCandidates(ranges, mask, 1000);
        if(candidates.size() < 1000)
            cout << "Too few tuples. Try larger ranges again!" << endl;
        else
            break;
    }while(true);
    AlgorithmRunner runner(candidates);
    while(!runner.isFinished()){
        vector<Point> points = runner.nextPair();
        int option = getOption(points[0], points[1]);
        runner.choose(option);
    }
    Point result = runner.getResult();
    cout << "Your favorite car is: " << endl;
    print_point(result.raw_ptr());
}

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
        .constructor<vector<Point> &>()
        .function("isFinished", &AlgorithmRunner::isFinished)
        .function("nextPair", &AlgorithmRunner::nextPair)
        .function("getResult", &AlgorithmRunner::getResult)
        .function("choose", &AlgorithmRunner::choose)
        .function("numLeftPoints", &AlgorithmRunner::numLeftPoints);
        ;
}
