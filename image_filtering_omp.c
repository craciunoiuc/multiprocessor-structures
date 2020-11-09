#include <stdio.h>
#include <stdlib.h>

#include "./image_IO.h"

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
    u_int8_t **images;
    uint *widths;
    uint *heights;
    u_int8_t *brightness;
    int nr_of_images;

    rc = parse_args(argc, argv);
    if (rc != 0) {
        return 1;
    }

    nr_of_images = images_read(argv[1], &images, &widths, &heights, &brightness);
    if (nr_of_images < 1) {
        return -1;
    }
    images_write(nr_of_images, &images, &widths, &heights);

    return 0;
}