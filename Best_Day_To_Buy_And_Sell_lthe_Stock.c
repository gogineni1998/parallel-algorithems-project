#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define n 15

int a[] = {111, 123, 2343, 234, 5, 26, 17, 78, 49, 100,1,1000000,0,11000000,12};

int a2[1000];

int main(int argc, char *argv[])
{
    int pid, np, elements_per_process, n_elements_recived, best_price = -1;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    printf("processor %d of %d\n", pid, np);
    int sum = 0;
    if (pid == 0) {
        int index, i;
        int min_value = 2147483647;
        elements_per_process = n / np;
        if(elements_per_process == 1) {
            min_value = a[0];
            best_price = min_value;
        }
        else {
            for (i = 0; i < elements_per_process; i++) {
                if(min_value > a[i]) {
                    min_value = a[i];
                }
                else {
                    int temp = a[i] - min_value;
                    if(temp > best_price) {
                        best_price = temp;
                    }
                }
            }
        }
        printf("Minimum value calculated by root process : %d\n\n", min_value);
        if (np > 1) {
            for (i = 1; i < np - 1; i++)
            {
                index = i * elements_per_process;
                MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD); 
                MPI_Send(&a[index], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD); 
                if (i == 1)
                {
                    MPI_Send(&min_value, 1, MPI_INT, i, 1, MPI_COMM_WORLD); 
                }
            }
            if (i == 1){
                MPI_Send(&min_value, 1, MPI_INT, i, 1, MPI_COMM_WORLD); 
            }
            index = i * elements_per_process;
            int elements_left = n - index;
            MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&a[index], elements_left, MPI_INT, i, 0, MPI_COMM_WORLD);
            int tmp;
            // MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            // printf("imp %d\n",tmp);
            // if(min_value > tmp) {
            //     min_value = tmp;
            // }
            for(int i=1;i<np;i++) {
                MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
                if(tmp > best_price) {
                    best_price = tmp;
                }
            }
            printf("best price value of array is : %d\n\n", best_price);
        }
        else {
            printf("best price value of array is : %d\n\n", best_price);
        }
    }
    else
    {
        int min_value;
        MPI_Recv(&n_elements_recived, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&a2, n_elements_recived, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        if (pid == 1)
        {
            MPI_Recv(&min_value, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            printf("MinValue of %d from %d to %d\n", min_value, pid - 1, pid);
        }
        if (pid > 1)
        {
            MPI_Recv(&min_value, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            printf("MinValue of %d from %d to %d\n", min_value, pid - 1, pid);
        }

        int partial_array_minValue = min_value;
        for (int i = 0; i < n_elements_recived; i++)
        {
            if(partial_array_minValue > a2[i]) {
                partial_array_minValue = a2[i];
            }
            else {
                int temp = a2[i] - partial_array_minValue;
                if(temp > best_price) {
                    best_price = temp;
                }
            }
        }
        MPI_Send(&best_price, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        printf("Partial min value calculated by process %d : %d\n", pid, partial_array_minValue);
        if (pid < np - 1)
        {
            MPI_Send(&partial_array_minValue, 1, MPI_INT, pid + 1, 1, MPI_COMM_WORLD);
        }
        if (pid == np - 1)
        {
            MPI_Send(&partial_array_minValue, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}