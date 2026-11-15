#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>
#include <omp.h>

using namespace std;

double normal_cdf(double x)
{
    return 0.5 * erfc(-x / sqrt(2.0));
}

int main(int argc, char **argv)
{
    if (argc != 10)
    {
        fprintf(stderr, "Usage: %s seq|omp THREADS FILE GENERATOR S K RATE SIGMA TIME\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    int threads = atoi(argv[2]);
    char *generator = argv[4];
    double s = atof(argv[5]);
    double k = atof(argv[6]);
    double rate = atof(argv[7]);
    double sigma = atof(argv[8]);
    double time = atof(argv[9]);

    if ((strcmp(mode, "seq") && strcmp(mode, "omp")) || threads <= 0 ||
        s <= 0 || k <= 0 || sigma <= 0 || time <= 0)
    {
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }

    FILE *file = fopen(argv[3], "r");
    if (!file)
    {
        perror("Cannot open input");
        return 1;
    }
    long long n;
    if (fscanf(file, "%lld", &n) != 1 || n <= 0)
    {
        fprintf(stderr, "Invalid number of samples\n");
        fclose(file);
        return 1;
    }
    vector<double> z(n);
    for (long long i = 0; i < n; i++)
    {
        if (fscanf(file, "%lf", &z[i]) != 1 || !isfinite(z[i]))
        {
            fprintf(stderr, "Invalid sample\n");
            fclose(file);
            return 1;
        }
    }
    fclose(file);

    double d1 = (log(s / k) + (rate + sigma * sigma / 2) * time) / (sigma * sqrt(time));
    double d2 = d1 - sigma * sqrt(time);
    double exact = s * normal_cdf(d1) - k * exp(-rate * time) * normal_cdf(d2);

    double drift = (rate - sigma * sigma / 2) * time;
    double scale = sigma * sqrt(time);
    double sum = 0;
    int actual_threads = 1;
    omp_set_dynamic(0);
    double start = omp_get_wtime();

    if (!strcmp(mode, "seq"))
    {
        for (long long i = 0; i < n; i++)
        {
            double stock = s * exp(drift + scale * z[i]);
            sum += max(stock - k, 0.0);
        }
    }
    else
    {
#pragma omp parallel num_threads(threads) reduction(+ : sum)
        {
#pragma omp single
            {
                actual_threads = omp_get_num_threads();
            }

#pragma omp for schedule(static)
            for (long long i = 0; i < n; i++)
            {
                double stock = s * exp(drift + scale * z[i]);
                sum += max(stock - k, 0.0);
            }
        }
    }

    double price = exp(-rate * time) * sum / n;
    double seconds = omp_get_wtime() - start;
    double error = fabs(price - exact);

    // mode,generator,n,threads,price,exact,error,seconds
    printf("%s,%s,%lld,%d,%.12f,%.12f,%.12g,%.9f\n",
           mode, generator, n, actual_threads, price, exact, error, seconds);
    return 0;
}
