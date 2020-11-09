#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	if (!strcmp(argv[1], "")) {
		fprintf(stderr, "Invalid input file\n");
		return 3;
	}

	return 0;
}

void apply_filter(int nr_of_images, u_int8_t **brightness, u_int8_t ***images,
		uint **widths, uint **heights)
{

	for (int i = 0; i < nr_of_images; ++i) {
		u_int64_t image_size = 4LL * (*widths)[i] * (*heights)[i];
		float factor = 2.f * ((float)((*brightness)[i])) / 100;
#pragma omp parallel for shared(images)
		for (u_int64_t pixel = 0; pixel < image_size; pixel += 4LL) {
			if (!(*images)[i][pixel + 3]) continue;
			u_int16_t pixel_storage[3];
			pixel_storage[0] = (*images)[i][pixel] * factor;
			pixel_storage[1] = (*images)[i][pixel + 1] * factor;
			pixel_storage[2] = (*images)[i][pixel + 2] * factor;

			(*images)[i][pixel]     = (u_int8_t) (pixel_storage[0] > 255)? 255 : pixel_storage[0];
			(*images)[i][pixel + 1] = (u_int8_t) (pixel_storage[0] > 255)? 255 : pixel_storage[1];
			(*images)[i][pixel + 2] = (u_int8_t) (pixel_storage[0] > 255)? 255 : pixel_storage[2];
		}
	}
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

	apply_filter(nr_of_images, &brightness, &images, &widths, &heights);

	images_write(nr_of_images, &images, &widths, &heights);

    return 0;
}