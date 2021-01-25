#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Converts an integer to a binary string in big endian
static void integer_to_binary(uint number, char *str)
{
    char current_bit = 0;
    char step = 0;
    char temp_str[33] = {0, };
    char len;

    memset(str, 0, 33);


    while (number > 0) {
        current_bit = number & 1;
        str[step] = current_bit ? '1' : '0';
        number >>= 1;
        step++;
    }
    
    len = strlen(str) - 1;
    for (int i = len; i >= 0; --i) {
        temp_str[len - i] = str[i];
    }

    memcpy(str, temp_str, 33);
}

// Adds missing '0' characters shifting everything to the right
static inline void pad_left_zeroes(char *binary_number, int nr_zeroes)
{
    if (!nr_zeroes)
        return;

    for (int i = strlen(binary_number) - 1; i >= 0; --i) {
        binary_number[i + nr_zeroes] = binary_number[i];
    }
    memset(binary_number, '0', nr_zeroes);
}

static inline int shuffle(int input, int size)
{
    return ((input % 2 == 0) ?
            input / 2 : (1 << (size - 1) | (input / 2)));
}

// Simulate the network
static void network(uint start_int, uint stop_int, uint len, uint nr_floors)
{
    char start[33] = {0, }, stop[33] = {0, };
    char output[200] = {0, };
    char output_step = 0;
    uint current_floor = 1;
    char network_size = len;
    uint floor_start[100][2];

    // Generate the binary numbers to work on
    integer_to_binary(start_int, start);
    integer_to_binary(stop_int, stop);
    pad_left_zeroes(stop, len - strlen(stop));
    pad_left_zeroes(start, len - strlen(start));

    // For the first half, all connections can be direct and for each step
    // The value at that step is saved. Also, if the step is in a network
    // bigger than 2 the result is halved.
    for (output_step = 0; output_step < nr_floors / 2;
                                    ++output_step, --network_size) {
        output[output_step] = 'd';
        start_int = shuffle(start_int, network_size);
        floor_start[output_step][0] = start_int;
        if (start_int >= (1 << (len - output_step)) / 2 &&
            output_step < len / 2 && nr_floors > 3) {
            start_int -= (1 << (len - output_step)) / 2;
            floor_start[output_step][1] = 1;
        } else {    
            floor_start[output_step][1] = 0;
        }
    }

    // In the middle, connection is decided by the MSB
    if (start[0] == stop[0]) {
        output[output_step] = 'd';
    } else {
        output[output_step] = 'i';
        start_int ^= 1;
    }
    output_step++;
    floor_start[output_step][0] = start_int;
    floor_start[output_step][1] = 0;
    network_size = len;

    // Start from the opposite direction with the stop point and move left
    // At each step try to get in the same half of that level in a greedy manner
    // Halve the result if necessary.
    for (int i = nr_floors - 1; i >= output_step + 1; --i, --network_size) {
        if (stop_int & 1) {
            if (floor_start[nr_floors - i - 1][1]) {
                output[i] = 'd';
                stop_int = shuffle(stop_int, network_size);
            } else {
                output[i] = 'i';
                stop_int = shuffle(stop_int ^ 1, network_size);
            }
        } else {
            if (floor_start[nr_floors - i - 1][1]) {
                output[i] = 'i';
                stop_int = shuffle(stop_int ^ 1, network_size);
            } else {
                output[i] = 'd';
                stop_int = shuffle(stop_int, network_size);  
            }
        }
        if (stop_int >= (1 << len) / 2)
            stop_int -= (1 << len) / 2;
    }

    // The for above stops at the network the size 4x4
    // In this case if the numbers are equal just proceed
    // In the other case check the parity of the number and proceed accordingly
    // to match the result. If the for above has finished successfully
    // the 2 numbers should differ by only 1 bit, and that is the parity one.
    if (stop_int == start_int) {
        output[output_step] = 'd';
    } else {
        if (stop_int > start_int) {
            if (stop_int & 1) {
                output[output_step] = 'i';
                stop_int = shuffle(stop_int ^ 1, network_size);
            } else {
                output[output_step] = 'd';
                stop_int = shuffle(stop_int, network_size);
            }
        } else {
            if (stop_int & 1) {
                output[output_step] = 'd';
                stop_int = shuffle(stop_int, network_size);
            } else {
                output[output_step] = 'i';
                stop_int = shuffle(stop_int ^ 1, network_size);
            }
        }
    }

    // Print the result
    for (int i = 0; i < nr_floors; ++i) {
        if (output[i] == 'd')
            printf("direct ");
        else
            printf("invers ");
    }
    printf("\n");
}

// Returns the number of figures
static inline int number_of_figures(int number) {
    int figures = number ? 0 : 1;
    while (number != 0) {
        number /= 10;
        figures++;
    }
    return figures;
}

// Prints number in binary in fixed form
static void print_binary(uint number, uint length)
{
    char str[33];
    int binary_len;

    integer_to_binary(number, str);
    binary_len = strlen(str);
    while (binary_len < length) {
        printf("0");
        binary_len++;
    }
    printf("%s", str);
}

// Prints table with all possible connections
static inline void print_map(uint size, uint log_size)
{
    printf("Index  Start  Stop\n");
    for (int i = 0; i <= size; ++i) {
        printf("%d ", i);
        int nr_figs = number_of_figures(i);
        for (int j = 0; j < 5 - nr_figs; ++j) {
            printf(" ");
        }
        print_binary(i, log_size);
        printf("   ");
        print_binary(i, log_size);
        printf("\n");
    }
}

int main()
{
    uint list_size = 0;
    uint nr_floors = 0;
    uint nr_max = 0;
    uint max_pow = 0;
    uint nr_chosen_start[256] = {0, };
    uint nr_chosen_stop[256] = {0, };

    while (nr_max <= 0 || nr_max % 2 == 1) {
        printf("Enter network maximum dimension (max input)\n");
        scanf("%u", &nr_max);
    }

    max_pow = log2(nr_max);
    nr_floors = 2 * max_pow - 1;
    nr_max--;

    printf("The chosen network has %u floors and accepts values ", nr_floors);
    printf("between 0 and %u\n", nr_max);
    
    print_map(nr_max, max_pow);

    do {
        printf("Choose number of pairs to enter\n");
        scanf("%u", &list_size);
    } while (list_size == 0);

    printf("Enter pairs of input and output like: in1 out1 in2 out2...\n");
    for (int i = 0; i < list_size; ++i) {
        scanf("%u", &nr_chosen_start[i]);
        scanf("%u", &nr_chosen_stop[i]);

    }

    for (int i = 0; i < list_size; ++i) {
        printf("Starting point: ");
        print_binary(nr_chosen_start[i], max_pow);
        printf(" and Finish point: ");
        print_binary(nr_chosen_stop[i], max_pow);
        printf("\n");

        // Input processing done, starting shuffle
        network(nr_chosen_start[i], nr_chosen_stop[i], max_pow, nr_floors);
    }

    return 0;
}