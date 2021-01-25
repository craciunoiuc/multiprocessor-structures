#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int** a;         // Left
int** b;         // Right
int** c;         // Result
int matrix_size; // Size

pthread_barrier_t barrier;
pthread_spinlock_t** spin;

// Simulate parallel writing to memory
static void priority_sum(int to_add, int i, int j) {
    pthread_spin_lock(&spin[i][j]);
    c[i][j] += to_add;
    pthread_spin_unlock(&spin[i][j]);
}

// Args is a pointer to the id of the thread
static void *threaded_multiply(void *args)
{
    int id = *((int *) args);
    int i = (id / matrix_size) % matrix_size;
    int j = id % matrix_size;
    int k = id / (matrix_size * matrix_size);
    int partial_result;
    int first_element;
    int second_element;

    // All threads at the same time (theoretically)
    pthread_barrier_wait(&barrier);
    first_element = a[i][k];

    // All threads at the same time (theoretically)
    pthread_barrier_wait(&barrier);
    second_element = b[k][j];

    // Temporary storage
    partial_result = first_element * second_element;

    // Write to memory using sum
    priority_sum(partial_result, i, j);

    return NULL;
}

int main()
{
    // Read from file
    // First line: matrix_size
    // Next: matrix_size lines signifing a
    // Next: matrix_size lines signifing b
    const char* file = "data.in";
    FILE* in;
    int ret;
    
    in = fopen(file, "r");
    if (!in) {
        return 1;
    }
    
    // Read matrix size
    ret = fscanf(in, "%d", &matrix_size);
    if (matrix_size == 0) {
        printf("Matrix does not exist\n");
        return 2;
    }

    // Allocate space for each matrix
    a = calloc(matrix_size, sizeof(int *));
    b = calloc(matrix_size, sizeof(int *));
    c = calloc(matrix_size, sizeof(int *));
    spin = calloc(matrix_size, sizeof(pthread_spinlock_t *));
    if (!a || !b || !c || !spin) {
        printf("Malloc failed\n");
        return 12;
    }

    for (int i = 0; i < matrix_size; ++i) {
        a[i] = calloc(matrix_size, sizeof(int));
        b[i] = calloc(matrix_size, sizeof(int));
        c[i] = calloc(matrix_size, sizeof(int));
        spin[i] = calloc(matrix_size, sizeof(pthread_spinlock_t));
        if (!a[i] || !b[i] || !c[i] || !spin[i]) {
            printf("Malloc failed\n");
            return 12;
        }
    }

    // Read the input
    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            fscanf(in, "%d", &a[i][j]);
        }
    }
    
    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            fscanf(in, "%d", &b[i][j]);
        }
    }

    // Create the threads, the spinlock and the barrier and do parallel work
    const int nr_threads = matrix_size * matrix_size * matrix_size;
    int idx[nr_threads];
    pthread_t tid[nr_threads];

    ret = pthread_barrier_init(&barrier, NULL, nr_threads);
    if (ret != 0) {
        printf("pthread_barrier_init failed, %d\n", ret);
        return 1;
    }

    // One lock for each zone of memory
    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            ret = pthread_spin_init(&spin[i][j], 0);
            if (ret != 0) {
                printf("pthread_spinlock_init failed\n");
                return 1;
            }
        }
    }

    for (int i = 0; i < nr_threads; ++i) {
        idx[i] = i;
        ret = pthread_create(&tid[i], NULL, &threaded_multiply, &idx[i]);
        if (ret != 0) {
            printf("pthread_create_init failed\n");
            return 1;
        }
    }

    // Clear parallel structures
    for (int i = 0; i < nr_threads; ++i) {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            pthread_spin_destroy(&spin[i][j]);
        }
    }

    pthread_barrier_destroy(&barrier);

    // Print the result to console
    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            printf("%d ", c[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    // Clean up memory
    for (int i = 0; i < matrix_size; ++i) {
        free(a[i]); free(b[i]); free(c[i]); free(spin[i]);
    }
    free(a); free(b); free(c); free(spin);
    fclose(in);
    return 0;
}
