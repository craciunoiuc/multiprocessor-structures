#ifndef _IMAGE_IO_
#define _IMAGE_IO_

// The function receives the name of the input file and returns the decoded
// images with their respective heights and widths, and with the brightness
// that needs to be applied.
// It returns the number of images read.
int images_read(char *file_name, unsigned char ***images, unsigned int **widths,
		unsigned int **heights, unsigned char **brightness);

// The function receives the filtered images with their sizes and writes them
// to files.
void images_write(int images_nr, unsigned char ***images, unsigned int **widths,
		unsigned int **heights);

#endif /* _IMAGE_IO_ */