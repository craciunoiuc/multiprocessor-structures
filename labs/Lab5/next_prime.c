#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MASTER 0
// List size, change accordingly
#define INPUT_SIZE 100000
// Maximum number that rand can generate
#define MAX_NR 434324422
// Offset to get bigger numbers
#define OFFSET_NR 100000000
// Set to 0 to disable writing to output
#define PRINT_ENABLED 1


// Returns 1 is a number is prime and 0 otherwise
uint is_prime(uint x)
{
    if (x == 1 || x == 2 || x == 3 || x == 5 || x == 7) return 1;
    uint sqr = sqrt(x);
    for (int i  = 3; i <= sqr; i += 2) {
        if (x % i  == 0) {
            return 0;
        }
    }
    return 1;
}

// Returns the next prime number or 0 if an overflow is found
uint next_prime(uint input)
{
    if (input == 0) return 0;
    for (int i = (input & 1) ? input + 2: input + 1; i < __INT32_MAX__; i += 2)
        if (is_prime(i)) return i;
    return 0;
}

int main(int argc, char *argv[])
{
    int rank, ntasks, inbalance;
    uint *input_list, list_size;
    uint *to_calc;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    if (rank == MASTER) {
        inbalance = 0;

        list_size = INPUT_SIZE;

        if (list_size < ntasks) {
            inbalance += ntasks - list_size;
        }
        if ((list_size + inbalance) % ntasks != 0) {
            inbalance += ntasks - ((list_size + inbalance) % ntasks);
        }
        input_list = calloc(list_size + inbalance, sizeof(uint));
        if (input_list == NULL) {
            MPI_Finalize();
            exit(0);
        }
        for (int i = 0; i < list_size; ++i) {
            input_list[i] = OFFSET_NR + rand() % MAX_NR;
        }
        list_size += inbalance;
    }
    
    MPI_Bcast(&list_size, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    to_calc = calloc(list_size / ntasks, sizeof(uint));
    if (to_calc == NULL) {
        MPI_Finalize();
        exit(0);
    }

    //Scatter numbers to solve
    MPI_Scatter(input_list, list_size / ntasks, MPI_INT,
            to_calc, list_size / ntasks, MPI_INT, MASTER, MPI_COMM_WORLD);

    // Calculate primes
    for (int i = 0; i < list_size / ntasks; ++i) {
        to_calc[i] = next_prime(to_calc[i]);
    }

    // Gather the results
    MPI_Gather(to_calc, list_size / ntasks, MPI_INT,
        input_list, list_size / ntasks, MPI_INT, MASTER, MPI_COMM_WORLD);

#if PRINT_ENABLED
    if (rank == MASTER) {
        printf("The next primes are:\n");
        for (int i = 0; i < list_size - inbalance; ++i) {
            printf("%d ", input_list[i]);
        }
        printf("\n");
    }
#endif

    if (rank == MASTER)
        free(input_list);
    free(to_calc);

    MPI_Finalize();
    return 0;
}











