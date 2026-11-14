/*
make generate

make THREAD=1 TIME=10 input.txt

make test

Macbook Air M5 16 RAM / 512 GB / 10 CORES
THREAD  TIME     SEC       POINTS
1       100      51.73     500
2       100      49.15     500
3       100      43.13     500
4       100      35.50     500
5       100      34.56     500
6       100      32.89     500
7       100      30.52     500
8       100      32.03     500
9       100      32.23     500
10      100      32.53     500
11      100      32.61     500
12      100      33.29     500
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "pthread_barrier.h"
#include <math.h>
#include "timer.h"

#define G 6.67430e-11

int g_n;
int g_nthreads;
double g_t_end;
double g_dt = 0.001;
int g_nsteps;

double *g_m = NULL;
double *g_x = NULL;
double *g_y = NULL;
double *g_vx = NULL;
double *g_vy = NULL;

double *g_fx = NULL;
double *g_fy = NULL;

double *g_lfx = NULL;
double *g_lfy = NULL;

FILE *g_csv = NULL;

pthread_barrier_t g_barrier;

void read_input(const char *filename)
{
    FILE *fp = fopen(filename, "r");

    if (fscanf(fp, "%d", &g_n) != 1 || g_n <= 0)
    {
        fprintf(stderr, "bad n in %s\n", filename);
        exit(1);
    }

    g_m = malloc(g_n * sizeof(double));
    g_x = malloc(g_n * sizeof(double));
    g_y = malloc(g_n * sizeof(double));
    g_vx = malloc(g_n * sizeof(double));
    g_vy = malloc(g_n * sizeof(double));

    if (!g_m || !g_x || !g_y || !g_vx || !g_vy)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    for (int i = 0; i < g_n; i++)
    {
        if (fscanf(fp, "%lf %lf %lf %lf %lf",
                   &g_m[i],
                   &g_x[i], &g_y[i],
                   &g_vx[i], &g_vy[i]) != 5)
        {
            fprintf(stderr, "bad record for particle %d\n", i);
            exit(1);
        }
    }
    fclose(fp);
}

void *runner(void *arg)
{
    long rank = (long)arg;

    long total_pairs = (long)g_n * (g_n - 1) / 2;
    long chunk = (total_pairs + g_nthreads - 1) / g_nthreads;

    long p_start = rank * chunk;
    long p_end = p_start + chunk;

    if (p_end > total_pairs)
        p_end = total_pairs;

    double *lfx = g_lfx + rank * g_n;
    double *lfy = g_lfy + rank * g_n;

    for (int step = 0; step < g_nsteps; step++)
    {
        for (int i = 0; i < g_n; i++)
        {
            lfx[i] = 0.0;
            lfy[i] = 0.0;
        }

        if (p_start < p_end)
        {
            int p = 0;
            long rem = p_start;

            while (rem >= g_n - 1 - p)
            {
                rem -= g_n - 1 - p;
                p++;
            }

            int q = p + 1 + (int)rem;

            for (long idx = p_start; idx < p_end; idx++)
            {
                double dx = g_x[q] - g_x[p];
                double dy = g_y[q] - g_y[p];
                double r2 = dx * dx + dy * dy;

                if (r2 >= 1e-30)
                {
                    double r = sqrt(r2);
                    double r3 = r2 * r;

                    double coef = G * g_m[p] * g_m[q] / r3;

                    double fx = coef * dx;
                    double fy = coef * dy;

                    lfx[p] += fx;
                    lfy[p] += fy;

                    lfx[q] -= fx;
                    lfy[q] -= fy;
                }

                q++;

                if (q >= g_n)
                {
                    p++;
                    q = p + 1;
                }
            }
        }

        pthread_barrier_wait(&g_barrier);

        if (rank == 0)
        {
            for (int i = 0; i < g_n; i++)
            {
                g_fx[i] = 0.0;
                g_fy[i] = 0.0;
            }
            for (int t = 0; t < g_nthreads; t++)
            {
                double *tfx = g_lfx + t * g_n;
                double *tfy = g_lfy + t * g_n;
                for (int i = 0; i < g_n; i++)
                {
                    g_fx[i] += tfx[i];
                    g_fy[i] += tfy[i];
                }
            }
            for (int i = 0; i < g_n; i++)
            {
                double old_vx = g_vx[i];
                double old_vy = g_vy[i];

                g_x[i] += old_vx * g_dt;
                g_y[i] += old_vy * g_dt;

                g_vx[i] = old_vx + (g_fx[i] / g_m[i]) * g_dt;
                g_vy[i] = old_vy + (g_fy[i] / g_m[i]) * g_dt;
            }
            fprintf(g_csv, "%.10g", (step + 1) * g_dt);
            for (int i = 0; i < g_n; i++)
            {
                fprintf(g_csv, ",%.10g,%.10g", g_x[i], g_y[i]);
            }
            fprintf(g_csv, "\n");
        }
        pthread_barrier_wait(&g_barrier);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    double start, finish;
    pthread_t *threads;

    g_nthreads = strtol(argv[1], NULL, 10);
    g_t_end = atof(argv[2]);

    read_input(argv[3]);

    g_nsteps = ceil(g_t_end / g_dt);
    if (g_nsteps < 1)
        g_nsteps = 1;

    g_fx = calloc(g_n, sizeof(double));
    g_fy = calloc(g_n, sizeof(double));
    g_lfx = calloc(g_nthreads * g_n, sizeof(double));
    g_lfy = calloc(g_nthreads * g_n, sizeof(double));
    if (!g_fx || !g_fy || !g_lfx || !g_lfy)
    {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    pthread_barrier_init(&g_barrier, NULL, g_nthreads);
    threads = malloc(g_nthreads * sizeof(pthread_t));

    g_csv = fopen("output/data.csv", "w");
    fprintf(g_csv, "%.10g", 0.0);
    for (int i = 0; i < g_n; i++)
    {
        fprintf(g_csv, ",%.10g,%.10g", g_x[i], g_y[i]);
    }
    fprintf(g_csv, "\n");

    GET_TIME(start);
    for (long t = 0; t < g_nthreads; t++)
    {
        if (pthread_create(&threads[t], NULL, runner, (void *)t) != 0)
        {
            fprintf(stderr, "pthread_create failed\n");
            return 1;
        }
    }
    for (long t = 0; t < g_nthreads; t++)
    {
        pthread_join(threads[t], NULL);
    }
    GET_TIME(finish);

    fclose(g_csv);

    printf("Elapsed time = %e seconds\n", finish - start);
    printf("n = %d, nsteps = %d, nthreads = %d\n", g_n, g_nsteps, g_nthreads);
    printf("Written to nbody.csv\n");

    pthread_barrier_destroy(&g_barrier);
    free(threads);
    free(g_m);
    free(g_x);
    free(g_y);
    free(g_vx);
    free(g_vy);
    free(g_fx);
    free(g_fy);
    free(g_lfx);
    free(g_lfy);

    return 0;
}
