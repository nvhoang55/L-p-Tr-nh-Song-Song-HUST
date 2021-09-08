#include <stdio.h>
#include <malloc.h>
#include "omp.h"

#define  M       20
#define  Time    1
#define  dt      0.01
#define  dx      0.1
#define  D       0.1
#define TOTAL_THREAD 4

//=========================
void DisplayArray(float T[], int size)
{
    for (int i = 0; i < size; i++)
        printf("  %.2f", T[i]);
    printf("\n");

}

//=========================
void Write2File(float *T, int size)
{
    FILE *result = fopen("result1.txt", "a");
    int i;

    for (i = 0; i < size; i++)
    {
        fprintf(result, "%lf", *(T + i));
        fprintf(result, "\n");
    }

    fclose(result);
}

//=========================
void KhoiTao(float T[])
{
    for (int i = 0; i < M; i++)
        T[i] = 25.0;
}

//=========================
void DHB2(float T[], float dT[], int start, int stop)
{
    int i;
    float c, l, r;
    for (i = start; i < stop; i++)
    {
        c = *(T + i);
        l = (i == 0) ? 100.0 : T[i - 1];
        r = (i == M - 1) ? 25.0 : T[i + 1];
        dT[i] = (r - 2 * c + l) / (dx * dx);
    }
}

//=========================
int main()
{
    int i, t, Ntime;

    float T[M];
    float dT[M];

    KhoiTao(T);

    printf("Gia tri khoi tao:\n");
    DisplayArray(T, M);
    Write2File(T, M);

    Ntime = Time / dt;
    int id, start, stop;
    int Mc = M / TOTAL_THREAD;

    omp_set_num_threads(TOTAL_THREAD);

#pragma omp parallel default(none) private(i, start, stop, id, t) shared(Mc, Ntime, T, dT)
    {
        id = omp_get_thread_num();

        start = id * Mc;
        stop = start + Mc;

        for (t = 0; t < Ntime; t++)
        {
            DHB2(T, dT, start, stop);

#pragma omp barrier
            for (i = start; i < stop; i++)
            {
                T[i] = T[i] + D * dt * (dT[i]);
            }

            Write2File(T, M);
        }
    }
    printf("Result of C:\n");
    DisplayArray(T, M);

    return 0;
}
