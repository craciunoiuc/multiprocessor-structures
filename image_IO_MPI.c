#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "./lode/lodepng.h"

#define MASTER 0

static inline int max(int a, int b)
{
	return (a > b) ? a : b;
}

// Receives a file and allocates memory to store all images in a matrix
// Returns the number of images read
int images_read(char *file_name, unsigned char ***images, unsigned int **widths,
		unsigned int **heights, unsigned char **brightness)
{
	FILE *input_file;
	int number_of_images = 0;
	unsigned char **encoded_images;
	size_t *encoded_images_sz;
	int rc = 0;
	int rank, ntasks;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

	if (rank == MASTER) {
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
			MPI_Finalize();
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
	}
	if (rank >= number_of_images) {
		return 0;
	}

	MPI_Bcast(&number_of_images, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	int to_recv = number_of_images / ntasks;
	if (rank + 1 == ntasks) {
		to_recv = (number_of_images % ntasks) ?
					to_recv + number_of_images % ntasks : to_recv;
	}
	if (rank == MASTER) {
		
		int vec_pos = 0;
		for (int i = 1; i < ntasks; ++i) {
			int to_send = number_of_images / ntasks;
			if (i + 1 == ntasks) {
				to_send = (number_of_images % ntasks) ?
							to_send + number_of_images % ntasks : to_send;
			}
			MPI_Send((*brightness) + vec_pos, to_send,
					MPI_BYTE, i, MASTER, MPI_COMM_WORLD);
			MPI_Send((encoded_images_sz) + vec_pos, to_send,
					MPI_UNSIGNED_LONG, i, MASTER, MPI_COMM_WORLD);
			for (int img_nr = vec_pos; img_nr < to_send; ++img_nr) {
				MPI_Send((encoded_images[img_nr]), to_send,
					MPI_BYTE, i, MASTER, MPI_COMM_WORLD);
			}
			vec_pos += to_send;
		}
	} else {
		*images            = malloc(to_recv * sizeof(unsigned char *));
		*brightness        = malloc(to_recv * sizeof(unsigned char *));
		*widths            = malloc(to_recv * sizeof(unsigned int *));
		*heights           = malloc(to_recv * sizeof(unsigned int *));
		encoded_images     = malloc(to_recv * sizeof(unsigned char *));
		encoded_images_sz  = malloc(to_recv * sizeof(size_t));
		if (!*images || !*brightness || !*widths ||
		!*heights || !encoded_images || !encoded_images_sz) {
			MPI_Finalize();
			return -ENOMEM;
		}

		for (int i = 0; i < to_recv; ++i) {

			MPI_Recv((*brightness), to_recv, MPI_BYTE, MASTER, MASTER,
					MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			MPI_Recv((encoded_images_sz), to_recv, MPI_UNSIGNED_LONG, MASTER,
					MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			for (int img_nr = 0; img_nr < to_recv; ++img_nr) {
				MPI_Recv((encoded_images) + img_nr, to_recv, MPI_BYTE, MASTER,
					MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
		}
	}

	for (int i = 0; i < to_recv; ++i) {
		rc = lodepng_decode32(&(*images)[i], &(*widths)[i],
		&(*heights)[i], encoded_images[i], encoded_images_sz[i]);
		if (rc) {
			printf("decode error: %s\n", lodepng_error_text(rc));
		}
	}

	if (rank == MASTER) {
		fclose(input_file);
		for (int i = 0; i < number_of_images; ++i) {
			free(encoded_images[i]);
		}
	}
	free(encoded_images_sz);
	free(encoded_images);
	return (rank == MASTER) ? number_of_images : to_recv;
}

// Receives images to write, writes them to files in the format
// "output_imageX.png" and frees the memory
void images_write(int images_nr, unsigned char ***images, unsigned int **widths,
		unsigned int **heights)
{
	int rc;
	int total_images = images_nr;
	char *image_prefix = "output/output_image";
	char *image_suffix = ".png";
	unsigned char **encoded_images;
	size_t *encoded_images_sz;
	int rank, ntasks;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

	if (rank == MASTER) {
		images_nr /= ntasks;
	}

	encoded_images     = malloc(images_nr * sizeof(unsigned char *));
	encoded_images_sz  = malloc(images_nr * sizeof(size_t));
	if (!encoded_images || !encoded_images_sz) {
		perror("Image Writing failed");
		exit(ENOMEM);
	}

	for (int i = 0; i < images_nr; ++i) {
		rc = lodepng_encode32(&encoded_images[i], &encoded_images_sz[i],
			(*images)[i], (*widths)[i],(*heights)[i]);
		if(rc) {
			printf("encode error: %s\n", lodepng_error_text(rc));
		}
	}

	if (rank == MASTER) {
		for (int i = 1; i < ntasks; ++i) {
			int to_recv = total_images / ntasks;
			int start = to_recv * i;
			if (i + 1 == ntasks) {
				to_recv = (total_images % ntasks) ?
							to_recv + total_images % ntasks : to_recv;
			}
			MPI_Recv((*widths) + start, to_recv, MPI_UNSIGNED, i,
					MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv((*heights) + start, to_recv, MPI_UNSIGNED, i,
					MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			for (int j = 0; j < to_recv; ++j) {
				MPI_Recv((*images)[start + j],
					4 * (*widths)[start + j] * (*heights)[start + j],
					MPI_BYTE, i, MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
		}
	} else {
		MPI_Send((*widths), images_nr, MPI_UNSIGNED,
				MASTER, MASTER, MPI_COMM_WORLD);
		MPI_Send((*heights), images_nr, MPI_UNSIGNED,
				MASTER, MASTER, MPI_COMM_WORLD);
		for (int i = 0; i < images_nr; ++i) {
			MPI_Send((*images)[i], 4 * (*widths)[i] * (*heights)[i], MPI_BYTE,
					MASTER, MASTER, MPI_COMM_WORLD);
		}
	}

	// Write all to files
	if (rank == MASTER) {
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
	} else {
		for (int i = 0; i < images_nr; ++i) {
			free(encoded_images[i]);
			free((*images)[i]);
		}
	}

	// Cleanup
	free(encoded_images);
	free(encoded_images_sz);
	free(*images);
	free(*heights);
	free(*widths);
}