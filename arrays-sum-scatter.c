#include <stdio.h>
#include <mpich/mpi.h>
#include <malloc.h>

#define NUM 20

void print(int *A, int *B, int *C)
{
    for (int i = 0; i < NUM; i++)
    {
        printf("A[%d]: %d", i, *(A + i));
        printf("\tB[%d]: %d", i, *(B + i));
        printf("\tC[%d]: %d\n", i, *(C + i));
    }
}

int main(int argc, char **argv)
{
    int i, rank, size;
    int *first, *second, *sum, *sub_first, *sub_second, *sub_sum;
    int cal_per_thread;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    cal_per_thread = NUM / size;

    // Initialize data at rank 0
    if (rank == 0)
    {
        first = (int *) malloc(NUM * sizeof(int));
        second = (int *) malloc(NUM * sizeof(int));
        sum = (int *) malloc(NUM * sizeof(int));

        for (i = 0; i < NUM; ++i)
        {
            first[i] = i;
            second[i] = i * 2;
        }
    }

    sub_first = (int *) malloc(cal_per_thread * sizeof(int));
    sub_second = (int *) malloc(cal_per_thread * sizeof(int));
    sub_sum = (int *) calloc(cal_per_thread, sizeof(int));

    // Split and send "cal_per_thread" elements of first, second to sub_first, sub_second
    MPI_Scatter(first, cal_per_thread, MPI_INT,
                sub_first, cal_per_thread, MPI_INT,
                0, MPI_COMM_WORLD);
    MPI_Scatter(second, cal_per_thread, MPI_INT,
                sub_second, cal_per_thread, MPI_INT,
                0, MPI_COMM_WORLD);

    // Calculating
    for (i = 0; i < cal_per_thread; ++i)
    {
        sub_sum[i] = sub_first[i] + sub_second[i];
    }

    // Send back to sum
    MPI_Gather(sub_sum, cal_per_thread, MPI_INT,
               sum, cal_per_thread, MPI_INT,
               0, MPI_COMM_WORLD);

    // Print result
    if (rank == 0)
    {
        print(first, second, sum);
    }

    MPI_Finalize();
    return 0;
}
