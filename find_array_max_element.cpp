#include <omp.h>
#include <stdio.h>
#include <malloc.h>

#define SIZE 10

int main()
{
    int id, total_thread, sub_size;
    int start, stop;
    int max = 0;
    int *array = (int *) malloc(SIZE * sizeof(int));

    for (int i = 0; i < SIZE; ++i)
    {
        array[i] = i;
    }

    omp_set_num_threads(5);
    max = array[0];

#pragma omp parallel default(none) private(id, total_thread, start, stop) shared(array, sub_size, max)
    {
        id = omp_get_thread_num();
        total_thread = omp_get_num_threads();

        sub_size = SIZE / total_thread;
        start = id * sub_size;
        stop = start + sub_size;

        for (int i = start + 1; i < stop; ++i)
        {
            if (max < array[i]) max = array[i];
        }
    };

    printf("Max is %d ", max);
}
