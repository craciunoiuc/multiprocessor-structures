#include "mpi.h"
#include "./image_filtering.h"
#include "./image_IO.h"

#define MASTER 0

void apply_filter(int nr_of_images, u_int8_t **brightness, u_int8_t ***images,
		uint **widths, uint **heights)
{

	for (int i = 0; i < nr_of_images; ++i) {
		u_int64_t image_size = 4LL * (*widths)[i] * (*heights)[i];
		if (image_size == 0LL) continue;
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
}

int main(int argc, char *argv[])
{
	u_int8_t **images, **images_recv;
	uint *widths, *widths_recv;
	uint *heights, *heights_recv;
	u_int8_t *brightness, *brightness_recv;
	int nr_of_images;
	int rank;
	int ntasks, inbalance;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

	if (rank == MASTER) {
		if (parse_args(argc, argv) != 0) {
			MPI_Finalize();
			return -1;
		}
		nr_of_images = images_read(argv[1], &images, &widths, &heights, &brightness);
		if (nr_of_images < 1) {
			MPI_Finalize();
			return -1;
		}
		// Pad arrays with 0-es and ignore those that have width and height 0
        	inbalance = 0;

		if (nr_of_images < ntasks) {
			inbalance += ntasks - nr_of_images;
		}
		if ((nr_of_images + inbalance) % ntasks != 0) {
			inbalance += ntasks - ((nr_of_images + inbalance) % ntasks);
		}
		nr_of_images += inbalance;
		// concat padding
		uint *aux;
		aux = calloc(nr_of_images, sizeof(uint));
		memcpy(aux, widths, (nr_of_images - inbalance) * sizeof(uint));
		free(widths);
		widths = aux;

		aux = calloc(nr_of_images, sizeof(uint));
		memcpy(aux, heights, (nr_of_images - inbalance) * sizeof(uint));
		free(heights);
		heights = aux;
		
		u_int8_t *auxb = calloc(nr_of_images, sizeof(u_int8_t));
		memcpy(auxb, brightness, (nr_of_images - inbalance) * sizeof(u_int8_t));
		free(brightness);
		brightness = auxb;
	}

// Broadcast nr_of_images
	MPI_Bcast(&nr_of_images, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	images_recv     = calloc(nr_of_images / ntasks, sizeof(u_int8_t *));
	brightness_recv = calloc(nr_of_images / ntasks, sizeof(u_int8_t));
	widths_recv     = calloc(nr_of_images / ntasks, sizeof(uint));
	heights_recv    = calloc(nr_of_images / ntasks, sizeof(uint));
	if (!images_recv || !brightness_recv || !widths_recv || !heights_recv) {
		perror("Alloc after broadcast");
		MPI_Finalize();
		return -1;
	}


	MPI_Scatter(brightness, nr_of_images / ntasks, MPI_BYTE, brightness_recv,
		nr_of_images / ntasks, MPI_BYTE, MASTER, MPI_COMM_WORLD);
	MPI_Scatter(widths, nr_of_images / ntasks, MPI_UNSIGNED, widths_recv,
		nr_of_images / ntasks, MPI_UNSIGNED, MASTER, MPI_COMM_WORLD);
	MPI_Scatter(heights, nr_of_images / ntasks, MPI_UNSIGNED, heights_recv,
		nr_of_images / ntasks, MPI_UNSIGNED, MASTER, MPI_COMM_WORLD);

	int start, end, slice;
	slice = nr_of_images / ntasks;
	start = rank * slice;
	end   = (rank + 1) * slice;
	for (int i = 0; i < slice; ++i) {
		
	}
	// Send from Master and receive in the rest
	if (rank == MASTER) {
		// Send equal sizes of images to each host
		for (int i = end; i < nr_of_images; ++i) {
			// If empty matrix should be sent, send 0
			if (i > nr_of_images - inbalance) {
				MPI_Send(images_recv[i % slice], 0,
				MPI_BYTE, i / slice, MASTER, MPI_COMM_WORLD);
			} else {
				MPI_Send(images[i], 4 * widths[i] * heights[i],
				MPI_BYTE, i / slice, MASTER, MPI_COMM_WORLD);
			}
		}
	} else {
		// For every image allocate space and receive the images
		for (int i = 0; i < slice; ++i) {
			uint size = 4 * heights_recv[i] * widths_recv[i];
			// Allocate space
			if (size > 0) {
				images_recv[i] = malloc(size * sizeof(u_int8_t));
			}
			// Receive them
			MPI_Recv(images_recv[i], size, MPI_BYTE, MASTER, MASTER,
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	if (rank == MASTER) {
		for (int i = 0; i < slice; ++i) {
			images_recv[i] = malloc(4 *
				heights_recv[i] * widths_recv[i]);
			memcpy(images_recv[i], images[i],
				4 * heights_recv[i] * widths_recv[i]);
		}
	}

	apply_filter(nr_of_images / ntasks, &brightness_recv, &images_recv,
			&widths_recv, &heights_recv);

// Gather images

	if (rank == MASTER) {
		for (int i = start; i < end; ++i) {
			memcpy(images[i], images_recv[i],
				4 * heights_recv[i] * widths_recv[i]);
		}
		for (int i = end; i < nr_of_images; ++i) {
			if (i >= (nr_of_images - inbalance)) continue;

			MPI_Recv(images[i], 4 * widths[i] * heights[i], MPI_BYTE,
			i / slice, MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	} else {
		for (int i = 0; i < slice; ++i) {
			if (4 * widths_recv[i] * heights_recv[i] == 0) continue;

			MPI_Send(images_recv[i],
				4 * widths_recv[i] * heights_recv[i],
				MPI_BYTE, MASTER, MASTER, MPI_COMM_WORLD);
		}
	}

	for (int i = 0; i < slice; ++i) {
		free(images_recv[i]);
	}
	free(images_recv);
	free(brightness_recv);

	if (rank == MASTER) {
		free(brightness);
		nr_of_images -= inbalance;
		images_write(nr_of_images, &images, &widths, &heights);
	}

	MPI_Finalize();
	return 0;
}