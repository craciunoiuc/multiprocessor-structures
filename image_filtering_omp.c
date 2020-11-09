#include "./image_filtering.h"
#include "./image_IO.h"

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

			(*images)[i][pixel] = (pixel_storage[0] > 255) ?
					255 : pixel_storage[0];
			(*images)[i][pixel + 1] = (pixel_storage[1] > 255) ?
					255 : pixel_storage[1];
			(*images)[i][pixel + 2] = (pixel_storage[2] > 255) ?
					255 : pixel_storage[2];
		}
	}
	free(*brightness);
}

int main(int argc, char *argv[])
{
	u_int8_t **images;
	uint *widths;
	uint *heights;
	u_int8_t *brightness;
	int nr_of_images;

	if (parse_args(argc, argv) != 0) {
		return -1;
	}

	nr_of_images = images_read(argv[1], &images, &widths, &heights, &brightness);
	if (nr_of_images < 1) {
		return -1;
	}

	apply_filter(nr_of_images, &brightness, &images, &widths, &heights);

	images_write(nr_of_images, &images, &widths, &heights);

	return 0;
}