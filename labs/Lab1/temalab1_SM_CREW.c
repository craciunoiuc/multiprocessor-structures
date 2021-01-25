#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int **a;         // Left
int **b;         // Right
int **c;         // Result
int ***v;        // Auxiliary
int matrix_size; // Size

pthread_barrier_t barrier;

static int simple_log2(int nr) {
    for (int i = 0; i <= 31; ++i) {
        if (nr < (1 << i)) {
            return i - 1;
        }
    }
    return -1;
}

static void parallel_reduce(int i, int j, int k)
{
    int log = simple_log2(matrix_size);
    for (int m = 0; m < log; ++m) {
        pthread_barrier_wait(&barrier);
        if (k % (1 << (m + 1)) == 0) {
            v[i][j][k] += v[i][j][k + (1 << m)];
        }
    }
}

// Args is a pointer to the id of the thread
static void *threaded_multiply(void *args)
{
    int id = *((int *) args);
    int i = (id / matrix_size) % matrix_size;
    int j = id % matrix_size;
    int k = id / (matrix_size * matrix_size);
    int first_element;
    int second_element;

    // All threads at the same time (theoretically)
    pthread_barrier_wait(&barrier);
    first_element = a[i][k];

    // All threads at the same time (theoretically)
    pthread_barrier_wait(&barrier);
    second_element = b[k][j];

    // Temporary storage in shared v (each thread writes in one zone)
    v[i][j][k] = first_element * second_element;

    parallel_reduce(i, j, k);

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
    v = calloc(matrix_size, sizeof(int **));
    if (!a || !b || !v) {
        printf("Malloc failed\n");
        return 12;
    }

    for (int i = 0; i < matrix_size; ++i) {
        a[i] = calloc(matrix_size, sizeof(int));
        b[i] = calloc(matrix_size, sizeof(int));
        v[i] = calloc(matrix_size, sizeof(int *));
        if (!a[i] || !b[i] || !v[i]) {
            printf("Malloc failed\n");
            return 12;
        }
    }

    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++ j) {
            v[i][j] = calloc(matrix_size, sizeof(int));
            if (!v[i][j]) {
                printf("Malloc failed\n");
                return 12;
            }
        }
    }

    // Read the matrixes
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

    // Create the threads, the mutex and the barrier and do parallel work
    const int nr_threads = matrix_size * matrix_size * matrix_size;
    int idx[nr_threads];
    pthread_t tid[nr_threads];

    ret = pthread_barrier_init(&barrier, NULL, nr_threads);
    if (ret != 0) {
        printf("pthread_barrier_init failed, %d\n", ret);
        return 1;
    }

    for (int i = 0; i < nr_threads; ++i) {
        idx[i] = i;
        ret = pthread_create(&tid[i], NULL, &threaded_multiply, &idx[i]);
        if (ret != 0) {
            printf("pthread_create_init failed\n");
            return 1;
        }
    }

    // Clean up parallel structures
    for (int i = 0; i < nr_threads; ++i) {
        pthread_join(tid[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    // Write output to console
    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            printf("%d ", v[i][j][0]);
        }
        printf("\n");
    }
    printf("\n");

    // Clean up memory
    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            free(v[i][j]);
        }
    }

    for (int i = 0; i < matrix_size; ++i) {
        free(a[i]); free(b[i]); free(v[i]);
    }

    free(a); free(b); free(v);
    fclose(in);
    return 0;
}
