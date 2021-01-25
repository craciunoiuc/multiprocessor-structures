#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    for (int i = strlen(binary_number) - 1; i >= 0; --i) {
        binary_number[i + nr_zeroes] = binary_number[i];
    }
    memset(binary_number, '0', nr_zeroes);
}

// Simulate shuffle
static void shuffle(uint start_int, uint stop_int)
{
    char start[33] = {0, }, stop[33] = {0, };
    integer_to_binary(start_int, start);
    integer_to_binary(stop_int, stop);
    char start_len = strlen(start);
    char stop_len = strlen(stop);
    char len = start_len;

    if (start_len > stop_len) {
        len = start_len;
        pad_left_zeroes(stop, start_len - stop_len);
    }
    if (stop_len > start_len) {
        len = stop_len;
        pad_left_zeroes(start, stop_len - start_len);
    }

    for (int i = 0; i < len; ++i) {
        printf("Floor %d: ", len - i - 1);
        if (start[i] == stop[i]) {
            printf("direct connection\n");
        } else {
            printf("inverse connection\n");
        }
    }
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
    uint nr_floors = 0;
    uint nr_max = 1;
    uint nr_chosen_start = 0;
    uint nr_chosen_stop = 0;

    while (nr_floors <= 0) {
        printf("Enter network dimension (number of floors)\n");
        scanf("%u", &nr_floors);
    }

    nr_max = (nr_max << nr_floors) - 1;

    printf("The chosen network has %u floors and accepts values ", nr_floors);
    printf("between 0 and %u\n", nr_max);
    
    print_map(nr_max, nr_floors);

    do {
        printf("Choose a start index for the shuffle\n");
        scanf("%u", &nr_chosen_start);
    } while (nr_chosen_start > nr_max);

    do {
        printf("Choose a stop index for the shuffle\n");
        scanf("%u", &nr_chosen_stop);
    } while (nr_chosen_stop > nr_max);

    printf("Starting point: ");
    print_binary(nr_chosen_start, nr_floors);
    printf(" and Finish point: ");
    print_binary(nr_chosen_stop, nr_floors);
    printf("\n");

    // Input processing done, starting shuffle
    shuffle(nr_chosen_start, nr_chosen_stop);

    return 0;
}