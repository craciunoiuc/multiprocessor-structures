#include "./image_filtering.h"
#include "./image_IO.h"

#include <pthread.h>

// TODO add your own number
#define NUM_THREADS 8

static inline u_int64_t max(u_int64_t a, u_int64_t b)
{
	return (a > b) ? a : b;
}

// Data structure that is given to the threads
typedef struct {
    u_int8_t **images;
	uint *widths;
	uint *heights;
	u_int8_t *brightness;
	int nr_of_images;
	int id;
} image_information_t;

// Each thread applies the brightness filter on the thread_id/num_threads part
// of an image
void *threaded_filter(void *args)
{
	image_information_t *image_data = (image_information_t *)args;

	
	for (int i = 0; i < image_data->nr_of_images; ++i) {
		u_int64_t image_size = 4LL * (image_data->widths)[i] *
			(image_data->heights)[i];
		float factor = 2.f * ((float)((image_data->brightness)[i])) / 100;

		u_int64_t slice = ((image_size / 4) / NUM_THREADS) * 4;
		u_int64_t start = image_data->id * slice;
		u_int64_t stop  = (image_data->id + 1) * slice;
		if (image_data->id + 1 == NUM_THREADS) {
			stop = max((image_data->id + 1) * slice, image_size);
		}

		for (u_int64_t pixel = start; pixel < stop; pixel += 4LL) {
			if (!(image_data->images)[i][pixel + 3]) continue;
			u_int16_t pixel_storage[3];
			pixel_storage[0] = (image_data->images)[i][pixel] * factor;
			pixel_storage[1] = (image_data->images)[i][pixel + 1] * factor;
			pixel_storage[2] = (image_data->images)[i][pixel + 2] * factor;

			(image_data->images)[i][pixel] = (pixel_storage[0] > 255) ?
					255 : pixel_storage[0];
			(image_data->images)[i][pixel + 1] = (pixel_storage[1] > 255) ?
					255 : pixel_storage[1];
			(image_data->images)[i][pixel + 2] = (pixel_storage[2] > 255) ?
					255 : pixel_storage[2];
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	u_int8_t **images;
	uint *widths;
	uint *heights;
	u_int8_t *brightness;
	int nr_of_images;

	pthread_t threads[NUM_THREADS];
	image_information_t image_data[NUM_THREADS];

	if (parse_args(argc, argv) != 0) {
		return -1;
	}

	nr_of_images = images_read(argv[1], &images, &widths, &heights, &brightness);
	if (nr_of_images < 1) {
		return -1;
	}

	for (int i = 0; i < NUM_THREADS; ++i) {
		image_data[i].brightness = brightness;
		image_data[i].heights = heights;
		image_data[i].widths = widths;
		image_data[i].nr_of_images = nr_of_images;
		image_data[i].images = images;
		image_data[i].id = i;
	}

	for (int i = 1; i < NUM_THREADS; ++i) {
		pthread_create(&threads[i], NULL,
				threaded_filter, &image_data[i]);
	}

	threaded_filter(&image_data[0]);

	
	for (int i = 1; i < NUM_THREADS; ++i) {
		pthread_join(threads[i], NULL);
	}
	images_write(nr_of_images, &images, &widths, &heights);
	free(brightness);
	return 0;
}