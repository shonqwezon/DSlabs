#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <omp.h>

using namespace std;

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("Usage: %s seq|omp N THREADS SEED\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    long long n = atoll(argv[2]);
    int threads = atoi(argv[3]);
    int seed = atoi(argv[4]);

    if (strcmp(mode, "seq") != 0 && strcmp(mode, "omp") != 0) {
        printf("Mode must be seq or omp\n");
        return 1;
    }

    if (n <= 0 || threads <= 0 || seed < 0) {
        printf("N and THREADS must be positive, SEED must be nonnegative\n");
        return 1;
    }

    long long hits = 0;
    int actual_threads = 1;

    omp_set_dynamic(0);

    double start = omp_get_wtime();

    if (strcmp(mode, "seq") == 0) {
        mt19937 generator(seed);
        uniform_real_distribution<double> distribution(-1.0, 1.0);

        for (long long i = 0; i < n; i++) {
            double x = distribution(generator);
            double y = distribution(generator);

            if (x * x + y * y <= 1.0) {
                hits++;
            }
        }
    } else {
        #pragma omp parallel num_threads(threads) reduction(+:hits)
        {
            int id = omp_get_thread_num();

            mt19937 generator(seed + id);
            uniform_real_distribution<double> distribution(-1.0, 1.0);

            #pragma omp single
            {
                actual_threads = omp_get_num_threads();
            }

            #pragma omp for schedule(static)
            for (long long i = 0; i < n; i++) {
                double x = distribution(generator);
                double y = distribution(generator);

                if (x * x + y * y <= 1.0) {
                    hits++;
                }
            }
        }
    }

    double elapsed = omp_get_wtime() - start;
    double pi = 4.0 * hits / n;

    // mode,n,threads,seed,hits,pi,seconds
    printf("%s,%lld,%d,%d,%lld,%.10f,%.9f\n",
           mode, n, actual_threads, seed, hits, pi, elapsed);

    return 0;
}
