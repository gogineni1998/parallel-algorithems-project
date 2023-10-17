#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

int a2[1000];

int main(int argc, char *argv[])
{
    FILE* ptr;
    ptr = fopen("input.txt", "r");
    int n, a[1025];
    char line[1024];
    int flag = 0,ind = 0;
    while (fgets(line, 1024, ptr) != NULL) {
        if(flag == 0) {
            n = atoi(line);
            flag = 1;
        }
        else {
            a[ind++] = atoi(line);
        }
    }
    int pid, np, elements_per_process, n_elements_recived, best_price = -1;
    MPI_Status status;
    double t1, t2;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    printf("Executing process %d of %d\n", pid, np);
    int sum = 0;
    if (pid == 0)
    {
        t1 = MPI_Wtime();
        int index, i;
        int min_value = 2147483647;
        elements_per_process = n / np;
        if (elements_per_process == 1)
        {
            min_value = a[0];
            best_price = min_value;
        }
        else
        {
            for (i = 0; i < elements_per_process; i++)
            {
                if (min_value > a[i])
                {
                    min_value = a[i];
                }
                else
                {
                    int temp = a[i] - min_value;
                    if (temp > best_price)
                    {
                        best_price = temp;
                    }
                }
            }
        }
        if (np > 1)
        {
            for (i = 1; i < np - 1; i++)
            {
                index = i * elements_per_process;
                MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&a[index], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
                if (i == 1)
                {
                    MPI_Send(&min_value, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                    //printf("Sending best_price from %d to %d : %d\n", pid, i, best_price);
                    MPI_Send(&best_price, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
                }
            }
            if (i == 1)
            {
                MPI_Send(&min_value, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                MPI_Send(&best_price, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            }
            index = i * elements_per_process;
            int elements_left = n - index;
            MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&a[index], elements_left, MPI_INT, i, 0, MPI_COMM_WORLD);
            int tmp;
            MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
            //printf("Received best price from %d to %d : %d\n", np-1, 0, tmp);
            if (tmp > best_price)
            {
                best_price = tmp;
            }
            t2 = MPI_Wtime();
            printf("*** best of the best price value of the data is : $ %d ***\n\n", best_price);
            printf("Elapsed time is %f\n", t2 - t1);
        }
        else
        {
            t2 = MPI_Wtime();
            printf("**** best of the best price value of the data is : $ %d ****\n\n", best_price);
            printf("Elapsed time is %f\n", t2 - t1);
        }
    }
    else
    {
        int min_value, prev_best_price;
        MPI_Recv(&n_elements_recived, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&a2, n_elements_recived, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&min_value, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&prev_best_price, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
        //printf("Received best price from %d to %d : %d\n",pid-1,pid,prev_best_price);
        int partial_array_minValue = min_value;
        for (int i = 0; i < n_elements_recived; i++)
        {
            if (partial_array_minValue > a2[i])
            {
                partial_array_minValue = a2[i];
            }
            else
            {
                int temp = a2[i] - partial_array_minValue;
                if (temp > best_price)
                {
                    best_price = temp;
                }
            }
        }
        if (pid < np - 1)
        {
            MPI_Send(&partial_array_minValue, 1, MPI_INT, pid + 1, 1, MPI_COMM_WORLD);
            if (best_price > prev_best_price)
            {
                //printf("Sending best_price from %d to %d : %d\n", pid, pid+1, best_price);
                MPI_Send(&best_price, 1, MPI_INT, pid + 1, 2, MPI_COMM_WORLD);
            }
            else
            {
                //printf("Sending best_price from %d to %d : %d\n", pid, pid+1, prev_best_price);
                MPI_Send(&prev_best_price, 1, MPI_INT, pid + 1, 2, MPI_COMM_WORLD);
            }
        }
        if (pid == np - 1)
        {
            MPI_Send(&partial_array_minValue, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            if (best_price > prev_best_price)
            {
                //printf("Sending best_price from %d to %d : %d\n", pid, 0, best_price);
                MPI_Send(&best_price, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            }
            else
            {
                //printf("Sending best_price from %d to %d : %d\n", pid, 0, prev_best_price);
                MPI_Send(&prev_best_price, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            }
        }
    }
    MPI_Finalize();
    return 0;
}