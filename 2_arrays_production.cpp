#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#define THREAD_NUM 2

#define M 2
#define N 2
#define P 2

void print_matrix(int **A, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            printf("\t%d", A[i][j]);
        printf("\n");
    }
}

int main()
{
    // Check if matrix row and thread num are valid
    if (M % THREAD_NUM != 0)
    {
        printf("Row number of matrix 1 mod total thread must be 0");
        exit(0);
    }

    // Random integer
    time_t t;
    srand((unsigned) time(&t));

    int id, total_thread, sub_matrix_row_num;
    int start, stop;
    int i, j;

    // Initialize matrices
    int *matrix_1[M], *matrix_2[N], *result[M];

    for (i = 0; i < M; i++)
        matrix_1[i] = (int *) malloc(N * sizeof(int));

    for (i = 0; i < N; i++)
        matrix_2[i] = (int *) malloc(P * sizeof(int));

    for (i = 0; i < M; i++)
        result[i] = (int *) calloc(P, sizeof(int));

    // Generate data matrix_1
    for (int i = 0; i < M; ++i)
    {
        for (j = 0; j < N; ++j)
        {
            matrix_1[i][j] = rand() % 50;
        }
    }

    // Generate data matrix_2
    for (int i = 0; i < N; ++i)
    {
        for (j = 0; j < P; ++j)
        {
            matrix_2[i][j] = rand() % 50;
        }
    }

    printf("1:\n");
    print_matrix(matrix_1, M, N);
    printf("2:\n");
    print_matrix(matrix_2, N, P);

    omp_set_num_threads(THREAD_NUM);

#pragma omp parallel default(none) private(id, total_thread, start, stop) shared(sub_matrix_row_num, result, matrix_1, matrix_2)
    {
        id = omp_get_thread_num();
        total_thread = omp_get_num_threads();

        // Split matrix_1 by row
        sub_matrix_row_num = M / total_thread;
        start = id * sub_matrix_row_num;
        stop = start + sub_matrix_row_num;

        // Iterate through each row of sub-matrix
        for (int i = start; i < stop; i++)
        {
            // Iterate through each column of matrix_2
            for (int j = 0; j < P; j++)
            {
                // Reset result[i][j] for calculating new position of result matrix
                result[i][j] = 0;

                // Iterate through each column of sub-matrix and row of matrix_2 (both are N)
                for (int k = 0; k < N; k++)
                {
                    // Production
                    result[i][j] += matrix_1[i][k] * matrix_2[k][j];
                }
            }
        }
    };

    printf("Result:\n");
    print_matrix(result, M, P);
}
