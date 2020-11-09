#ifndef _IMAGE_IO_
#define _IMAGE_IO_

int images_read(char *file_name, unsigned char ***images, unsigned int **widths,
		unsigned int **heights, unsigned char **brightness);
void images_write(int images_nr, unsigned char ***images, unsigned int **widths,
		unsigned int **heights);

#endif /* _IMAGE_IO_ */