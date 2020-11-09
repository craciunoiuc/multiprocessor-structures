#include <stdio.h>

int parse_args(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Not enough arguments: ./my-program input_file\n");
        return 1;
    }
    if (argc > 2) {
        fprintf(stderr, "Too many arguments: ./my-program input_file\n");
        return 2;
    }
    if (argv[1] == "") {
        fprintf(stderr, "Invalid input file\n");
        return 3;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int rc = 0;
    rc = parse_args(argc, argv);
    if (rc != 0) {
        return 1;
    }

    
    return 0;
}