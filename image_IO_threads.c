#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "./lode/lodepng.h"

// TODO add your own number of threads
#define NUM_THREADS 8

// Data structure that is given to the threads
typedef struct {
	int id;
	int number_of_images;
	size_t *encoded_images_sz;
	unsigned int **widths;
	unsigned int **heights;
	unsigned char ***images;
	unsigned char **encoded_images;
} thread_data_t;

static inline u_int64_t max(u_int64_t a, u_int64_t b)
{
	return (a > b) ? a : b;
}

// Parallel call to the image encoder
static void *threaded_encode(void *args)
{
	int rc;
	thread_data_t *info = args;
	int slice = info->number_of_images / NUM_THREADS;
	int start = info->id * slice;
	int stop  = (info->id + 1) * slice;
	if (info->id + 1 == NUM_THREADS) {
		stop = max((info->id + 1) * slice, info->number_of_images);
	}

	for (int i = start; i < stop; ++i) {
		rc = lodepng_encode32(&(info->encoded_images)[i],
			&(info->encoded_images_sz)[i], (*info->images)[i],
			(*info->widths)[i], (*info->heights)[i]);
		if(rc) {
			printf("encode error: %s\n", lodepng_error_text(rc));
		}
	}
	return NULL;
}

// Parallel call to the image decoder
static void *threaded_decode(void *args)
{
	int rc;
	thread_data_t *info = args;
	int slice = info->number_of_images / NUM_THREADS;
	int start = info->id * slice;
	int stop  = (info->id + 1) * slice;
	if (info->id + 1 == NUM_THREADS) {
		stop = max((info->id + 1) * slice, info->number_of_images);
	}

	for (int i = start; i < stop; ++i) {
		rc = lodepng_decode32(&(*info->images)[i],
			&(*info->widths)[i], &(*info->heights)[i],
			info->encoded_images[i], info->encoded_images_sz[i]);
		if (rc) {
			printf("decode error: %s\n", lodepng_error_text(rc));
		}
	}
	return NULL;
}

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

	pthread_t threads[NUM_THREADS];
	thread_data_t info[NUM_THREADS];

	for(int i = 0; i < NUM_THREADS; ++i) {
		info[i].id                = i;
		info[i].encoded_images    = encoded_images;
		info[i].encoded_images_sz = encoded_images_sz;
		info[i].heights           = heights;
		info[i].widths            = widths;
		info[i].images            = images;
		info[i].number_of_images  = number_of_images;
	}

	for (int i = 1; i < NUM_THREADS; ++i) {
		pthread_create(&threads[i], NULL, threaded_decode, &info[i]);
	}

	threaded_decode(&info[0]);

	for (int i = 1; i < NUM_THREADS; ++i) {
		pthread_join(threads[i], NULL);
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

	pthread_t threads[NUM_THREADS];
	thread_data_t info[NUM_THREADS];

	for(int i = 0; i < NUM_THREADS; ++i) {
		info[i].id                = i;
		info[i].encoded_images    = encoded_images;
		info[i].encoded_images_sz = encoded_images_sz;
		info[i].heights           = heights;
		info[i].widths            = widths;
		info[i].images            = images;
		info[i].number_of_images  = images_nr;
	}

	for (int i = 1; i < NUM_THREADS; ++i) {
		pthread_create(&threads[i], NULL, threaded_encode, &info[i]);
	}

	threaded_encode(&info[0]);

	for (int i = 1; i < NUM_THREADS; ++i) {
		pthread_join(threads[i], NULL);
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