#ifndef _IMAGE_FILTERING_
#define _IMAGE_FILTERING_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	if (!strcmp(argv[1], "")) {
		fprintf(stderr, "Invalid input file\n");
		return 3;
	}

	return 0;
}

void apply_filter(int nr_of_images, u_int8_t **brightness, u_int8_t ***images,
		uint **widths, uint **heights);

#endif /* _IMAGE_FILTERING_ */