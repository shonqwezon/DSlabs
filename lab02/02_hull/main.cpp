
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>
#include <omp.h>

using namespace std;

struct Point {
    double x, y;
};

// Both chains are stored from left to right.
struct Hull {
    vector<Point> lower;
    vector<Point> upper;
};

bool compare(Point a, Point b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
}

double cross(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - a.y)
         - (b.y - a.y) * (c.x - a.x);
}

void add_point(vector<Point> &chain, Point p, bool lower) {
    while (chain.size() >= 2) {
        int n = chain.size();
        double turn = cross(chain[n - 2], chain[n - 1], p);
        if (lower && turn > 0) break;
        if (!lower && turn < 0) break;
        chain.pop_back();
    }
    chain.push_back(p);
}

// Removing the inner vertices exposes the common supporting segment.
void merge_chain(vector<Point> &left, const vector<Point> &right,
                 bool lower) {
    for (Point p : right) {
        add_point(left, p, lower);
    }
}

Hull solve(const vector<Point> &points, int begin, int end,
           bool parallel, int depth) {
    Hull result;

    if (end - begin <= 32) {
        for (int i = begin; i < end; i++) {
            add_point(result.lower, points[i], true);
            add_point(result.upper, points[i], false);
        }
        return result;
    }

    int middle = begin + (end - begin) / 2;
    Hull left, right;

    if (parallel && depth > 0 && end - begin >= 10000) {
        #pragma omp task shared(left, points) firstprivate(begin, middle, depth)
        {
            left = solve(points, begin, middle, true, depth - 1);
        }

        #pragma omp task shared(right, points) firstprivate(middle, end, depth)
        {
            right = solve(points, middle, end, true, depth - 1);
        }

        #pragma omp taskwait
    } else {
        left = solve(points, begin, middle, false, 0);
        right = solve(points, middle, end, false, 0);
    }

    merge_chain(left.lower, right.lower, true);
    merge_chain(left.upper, right.upper, false);
    return left;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s seq|omp THREADS < points.txt > hull.txt\n", argv[0]);
        return 1;
    }

    bool parallel = strcmp(argv[1], "omp") == 0;
    int threads = atoi(argv[2]);
    if ((!parallel && strcmp(argv[1], "seq") != 0) || threads <= 0) {
        fprintf(stderr, "Invalid mode or thread count\n");
        return 1;
    }

    int n;
    if (scanf("%d", &n) != 1 || n <= 0 || n > 100000000) {
        fprintf(stderr, "N must be between 1 and 100000000\n");
        return 1;
    }

    vector<Point> points(n);
    for (int i = 0; i < n; i++) {
        if (scanf("%lf %lf", &points[i].x, &points[i].y) != 2) {
            fprintf(stderr, "Invalid point\n");
            return 1;
        }
    }

    omp_set_dynamic(0);
    int actual_threads = 1;
    Hull hull;
    double start = omp_get_wtime();

    sort(points.begin(), points.end(), compare);

    if (parallel) {
        #pragma omp parallel num_threads(threads)
        {
            #pragma omp single
            {
                actual_threads = omp_get_num_threads();
                int depth = 0;
                long long tasks = 1;
                while (tasks < 4LL * actual_threads) {
                    tasks *= 2;
                    depth++;
                }
                hull = solve(points, 0, n, true, depth);
            }
        }
    } else {
        hull = solve(points, 0, n, false, 0);
    }

    vector<Point> boundary = hull.lower;
    int upper_size = hull.upper.size();
    for (int i = upper_size - 2; i > 0; i--) {
        boundary.push_back(hull.upper[i]);
    }

    double seconds = omp_get_wtime() - start;

    printf("%d\n", (int) boundary.size());
    for (Point p : boundary) {
        printf("%.17g %.17g\n", p.x, p.y);
    }

    // Measurements go to stderr, leaving stdout in the required format.
    fprintf(stderr, "%s,%d,%d,%.9f\n",
            argv[1], n, actual_threads, seconds);
    return 0;
}
