#include <omp.h>
#include <stdio.h>
#include <malloc.h>

#define SIZE 4

int main()
{
    int id, total_thread, sub_size;
    int start, stop;
    int sum = 0;
    int *array = (int *) malloc(SIZE * sizeof(int));

    for (int i = 0; i < SIZE; ++i)
    {
        array[i] = i;
    }

    omp_set_num_threads(2);

#pragma omp parallel default(none) private(id, total_thread, start, stop) shared(array, sub_size, sum)
    {
        id = omp_get_thread_num();
        total_thread = omp_get_num_threads();

        sub_size = SIZE / total_thread;
        start = id * sub_size;
        stop = start + sub_size;

        printf("id: %d - start: %d - stop: %d\n", id, start, stop);

        for (int i = start; i < stop; ++i)
        {
            sum += *(array + i);
        }
    };

    printf("Sum from 0 to %d is %d ", SIZE, sum);
}
