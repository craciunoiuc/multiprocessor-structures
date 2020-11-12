#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "./lode/lodepng.h"

int images_read(char *file_name, unsigned char ***images, unsigned int **widths,
		unsigned int **heights, unsigned char **brightness)
{
	FILE *input_file;
	int number_of_images = 0;
	unsigned char **encoded_images;
	size_t *encoded_images_sz;
	int rc = 0;

	input_file = fopen(file_name, "r");
	if (input_file == NULL) {
		return -1;
	}

	// Read number of images
	fscanf(input_file, "%d", &number_of_images);

	if (number_of_images < 1) {
		return -1;
	}

	// Allocate memory for all image information
	*images            = malloc(number_of_images * sizeof(unsigned char *));
	*brightness        = malloc(number_of_images * sizeof(unsigned char *));
	*widths            = malloc(number_of_images * sizeof(unsigned int *));
	*heights           = malloc(number_of_images * sizeof(unsigned int *));
	encoded_images     = malloc(number_of_images * sizeof(unsigned char *));
	encoded_images_sz  = malloc(number_of_images * sizeof(size_t));
	if (!*images || !*brightness || !*widths ||
	!*heights || !encoded_images || !encoded_images_sz) {
		return -ENOMEM;
	}

	// Read brightness and load the image for each file name,
	// but do not decode it
	for (int i = 0; i < number_of_images; ++i) {
		char image_name[256];
		fscanf(input_file, "%hhu%%", &(*brightness)[i]);
		if ((*brightness)[i] > 100) {
			return -1;
		}
		fscanf(input_file, "%s", image_name);
		rc = lodepng_load_file(&encoded_images[i],
			&encoded_images_sz[i], image_name);
		if (rc) {
			printf("open error: %s\n", lodepng_error_text(rc));
			return -1;
		}
	}

#pragma omp parallel for shared(encoded_images, encoded_images_sz, images, widths, heights, number_of_images) private(rc)
	for (int i = 0; i < number_of_images; ++i) {
		rc = lodepng_decode32(&(*images)[i], &(*widths)[i],
		&(*heights)[i], encoded_images[i], encoded_images_sz[i]);
		if (rc) {
			printf("decode error: %s\n", lodepng_error_text(rc));
		}
	}

	fclose(input_file);
	for (int i = 0; i < number_of_images; ++i) {
		free(encoded_images[i]);
	}
	free(encoded_images_sz);
	free(encoded_images);
	return number_of_images;
}

void images_write(int images_nr, unsigned char ***images, unsigned int **widths,
		unsigned int **heights)
{
	int rc;
	char *image_prefix = "output/output_image";
	char *image_suffix = ".png";
	unsigned char **encoded_images;
	size_t *encoded_images_sz;

	encoded_images     = malloc(images_nr * sizeof(unsigned char *));
	encoded_images_sz  = malloc(images_nr * sizeof(size_t));
	if (!encoded_images || !encoded_images_sz) {
		perror("Image Writing failed");
		exit(ENOMEM);
	}

#pragma omp parallel for shared(encoded_images, encoded_images_sz, images, widths, heights, images_nr) private(rc)
	for (int i = 0; i < images_nr; ++i) {
		rc = lodepng_encode32(&encoded_images[i], &encoded_images_sz[i],
			(*images)[i], (*widths)[i],(*heights)[i]);
		if(rc) {
			printf("encode error: %s\n", lodepng_error_text(rc));
		}
	}

	// Write all to files
	for (int i = 0; i < images_nr; ++i) {
		char image_name[256];
		sprintf(image_name, "%s%d%s", image_prefix, i, image_suffix);

		rc = lodepng_save_file(encoded_images[i], encoded_images_sz[i],
				image_name);
		if(rc) {
			printf("save error: %s\n", lodepng_error_text(rc));
		}
		free(encoded_images[i]);
		free((*images)[i]);
	}

	// Cleanup
	free(encoded_images);
	free(encoded_images_sz);
	free(*images);
	free(*heights);
	free(*widths);
}