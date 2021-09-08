#include <stdio.h>
#include <stdlib.h>
#include <mpich/mpi.h>
#include <malloc.h>

int main(int argc, char **argv)
{
    srand(0);

    int size, sub_arr_size, _max, rank, nums_thread, *random_array, *sub_array;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nums_thread);

    // Generate random values for array at rank 0
    if (rank == 0)
    {
        printf("Enter array size = ");
        scanf("%d", &size);

        random_array = (int *) malloc(size * sizeof(int));

        for (int i = 0; i < size; i++)
        {
            random_array[i] = rand() % size;

            printf("%d ", random_array[i]);
        }
        printf("\n");
    }

    // Send size to all threads
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    sub_arr_size = size / nums_thread;

    // Split random_array into parts and send them to sub_array of all threads
    sub_array = (int *) malloc(sub_arr_size * sizeof(int));
    MPI_Scatter(random_array, sub_arr_size, MPI_INT,
                sub_array, sub_arr_size, MPI_INT,
                0, MPI_COMM_WORLD);

    // Find max using bubble algorithm
    for (int i = 0; i < sub_arr_size; i++)
    {
        sub_array[0] = sub_array[0] > sub_array[i] ? sub_array[0] : sub_array[i];
    }

    MPI_Reduce(sub_array, &_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Max value is: %d", _max);
    }

    MPI_Finalize();
    return 0;
}
