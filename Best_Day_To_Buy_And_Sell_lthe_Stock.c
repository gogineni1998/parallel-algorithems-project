#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define n 10

int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

int a2[1000];

int main(int argc, char *argv[])
{
    int pid, np, elements_per_process, n_elements_recived;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    printf("processor %d of %d\n", pid,np);
    int sum = 0;
    if (pid == 0)
    {
        int index, i;
        elements_per_process = n / np;
        for (i = 0; i < elements_per_process; i++)
        {
            sum = sum + a[i];
        }
        printf("Sum calculated by root process : %d\n\n", sum);
        if (np > 1)
        {
            for (i = 1; i < np - 1; i++)
            {
                index = i * elements_per_process;
                MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&a[index], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
                if (i == 1)
                {
                    MPI_Send(&sum, elements_per_process, MPI_INT, i, 1, MPI_COMM_WORLD);
                }
            }
            index = i * elements_per_process;
            int elements_left = n - index;
            MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&a[index], elements_left, MPI_INT, i, 0, MPI_COMM_WORLD);
            int tmp;
            MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            sum = tmp;
        }
        printf("Sum of array is : %d\n\n", sum);
    }
    else
    {
        int tmp_sum;
        if (pid == 1)
        {
            MPI_Recv(&tmp_sum, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            printf("sum of %d from %d to %d\n", tmp_sum, pid - 1, pid);
        }
        if (pid > 1)
        {
            MPI_Recv(&tmp_sum, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            printf("sum of %d from %d to %d\n", tmp_sum, pid - 1, pid);
        }
        MPI_Recv(&n_elements_recived, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&a2, n_elements_recived, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        int partial_sum = tmp_sum;
        for (int i = 0; i < n_elements_recived; i++)
        {
            partial_sum = partial_sum + a2[i];
        }
        printf("Partial sum calculated by process %d : %d\n", pid, partial_sum);
        if (pid < np - 1)
        {
            MPI_Send(&partial_sum, 1, MPI_INT, pid + 1, 1, MPI_COMM_WORLD);
        }
        if (pid == np - 1)
        {
            MPI_Send(&partial_sum, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}