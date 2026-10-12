/*
make generate

make only_visualize

make THREAD=1 POINTS=10000

make test

Macbook Air M5 16 RAM / 512 GB / 10 CORES
THREAD  POINTS       SEC
1       100000000    17.34
2       100000000    8.95
3       100000000    6.03
4       100000000    4.54
5       100000000    3.93
6       100000000    3.52
7       100000000    3.15
8       100000000    2.86
9       100000000    2.62
10      100000000    2.49
11      100000000    2.48
12      100000000    2.52
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"

#define MAX_ITER 1000

#define RE_MIN -2.0
#define RE_MAX 2.0
#define IM_MIN -2.0
#define IM_MAX 2.0

int g_nthreads;
int g_npoints;

double *g_points_re = NULL;
double *g_points_im = NULL;
int *g_result = NULL;

int is_in_mandelbrot(double c_re, double c_im)
{
    double z_re = 0.0;
    double z_im = 0.0;

    for (int i = 0; i < MAX_ITER; i++)
    {
        double z_re2 = z_re * z_re;
        double z_im2 = z_im * z_im;

        if (z_re2 + z_im2 > 4.0)
            return 0;

        double new_re = z_re2 - z_im2 + c_re;
        double new_im = 2.0 * z_re * z_im + c_im;
        z_re = new_re;
        z_im = new_im;
    }

    return 1;
}

void *runner(void *arg)
{
    long rank = (long)arg;
    int chunk = ceil((double)g_npoints / g_nthreads);
    int start = rank * chunk;
    int end = start + chunk;

    if (end > g_npoints)
        end = g_npoints;
    if (start > g_npoints)
        start = g_npoints;

    for (int i = start; i < end; i++)
    {
        g_result[i] = is_in_mandelbrot(g_points_re[i], g_points_im[i]);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    unsigned seed = 89;
    pthread_t *thread_handles;
    long total_found = 0;
    double start, finish;
    FILE *fp;

    g_nthreads = strtol(argv[1], NULL, 10);
    g_npoints = strtol(argv[2], NULL, 10);

    g_points_re = malloc(g_npoints * sizeof(double));
    g_points_im = malloc(g_npoints * sizeof(double));
    g_result = malloc(g_npoints * sizeof(int));
    if (!g_points_re || !g_points_im || !g_result)
    {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    for (int i = 0; i < g_npoints; i++)
    {
        double a = my_drand(&seed);
        double b = my_drand(&seed);
        g_points_re[i] = RE_MIN + a * (RE_MAX - RE_MIN);
        g_points_im[i] = IM_MIN + b * (IM_MAX - IM_MIN);
        g_result[i] = 0;
    }

    thread_handles = malloc(g_nthreads * sizeof(pthread_t));
    if (!thread_handles)
    {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    GET_TIME(start);
    for (int i = 0; i < g_nthreads; i++)
    {
        if (pthread_create(&thread_handles[i], NULL, runner, (void *)(long)i) != 0)
        {
            fprintf(stderr, "pthread_create failed\n");
            return 1;
        }
    }

    for (int i = 0; i < g_nthreads; i++)
    {
        pthread_join(thread_handles[i], NULL);
    }
    GET_TIME(finish);

    printf("Elapsed time = %e seconds\n", finish - start);

    fp = fopen("output/data.csv", "w");
    if (!fp)
    {
        fprintf(stderr, "cannot open output/data.csv for writing\n");
        return 1;
    }
    fprintf(fp, "re,im\n");
    for (int i = 0; i < g_npoints; i++)
    {
        if (g_result[i])
        {
            fprintf(fp, "%.17g,%.17g\n", g_points_re[i], g_points_im[i]);
            total_found++;
        }
    }
    fclose(fp);

    printf("Total points generated = %d\n", g_npoints);
    printf("Points in Mandelbrot set = %ld\n", total_found);
    printf("Written to output/data.csv\n");

    free(g_points_re);
    free(g_points_im);
    free(g_result);
    free(thread_handles);

    return 0;
}
